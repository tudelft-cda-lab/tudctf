# nim c --debugger:native --cpu:i386 -t:-m32 -l:-m32 brainfuck_jit.nim
import os
import strformat
import std/strutils
import std/tables
import std/hashes
import macros
import system

macro dd(input: untyped): untyped =
    let strCode = $toStrLit(input)
    parseStmt("""echo fmt"DEBUG: {$1 = }"""" % [strCode])

type Op = enum
    Increment
    Decrement
    MoveLeft
    MoveRight
    Input
    Output
    InvalidStartOfLoop
    StartOfLoop
    InvalidEndOfLoop
    EndOfLoop
    DebugPrint

type BlockEnderKind = enum
    CrashIfZero
    CrashIfNonZero
    JumpIfZero
    JumpIfNonZero
    WriteOutput
    ReadInput
    NothingSpecial
    BEDebugPrint
type BlockEnder = object
    case kind: BlockEnderKind
    of JumpIfZero, JumpIfNonZero:
        jump_block_index: int
    else:
        discard
type AdditionAndMoveBlock = object
    moves: seq[Op]
    total_to_the_right: int
    increments: Table[int, int]
    ender: BlockEnder


proc evaluate(blocks: seq[AdditionAndMoveBlock], input: string, verbose: bool) =
    # verbose = true
    var input_index = 0
    var values: seq[uint8]
    values.setLen(if verbose: 20 else: 8000)
    var cursor = 0
    var block_index = 0

    proc crash() =
        echo "Bruh"
        quit(-1)
    var i = 0
    while block_index < blocks.len:
        let b = blocks[block_index]
        i += 1
        # if i > 200:
        #     break
        if verbose and false:
            echo "Index: " & $block_index & " b.ender.kind: " & $b.ender.kind
            echo fmt"Cursor: {cursor} values: {values}"
        for offset, delta in b.increments:
            values[cursor + offset] += uint8(delta)
        cursor += b.total_to_the_right
        case b.ender.kind
        of CrashIfZero:
            if values[cursor] == 0:
                crash()
            block_index += 1
        of CrashIfNonZero:
            if values[cursor] != 0:
                crash()
            block_index += 1
        of JumpIfZero:
            if values[cursor] == 0:
                block_index = b.ender.jump_block_index
            else:
                block_index += 1
        of JumpIfNonZero:
            if values[cursor] != 0:
                block_index = b.ender.jump_block_index
            else:
                block_index += 1
        of WriteOutput:
            if verbose:
                echo fmt"OUTPUT: {char(values[cursor])}({values[cursor]})"
            # echo values[cursor]
            discard stdout.writeChars([char(values[cursor])], 0, 1)
            stdout.flushFile()
            block_index += 1
        of ReadInput:
            let input = if input_index < input.len: input[input_index] else: '\0'
            input_index += 1
            if verbose:
                echo fmt"INPUT = {int(input)}({input})"
            values[cursor] = uint8(input)
            block_index += 1
        of NothingSpecial:
            block_index += 1
        of BEDebugPrint:
            # echo "Index: " & $block_index & " b.ender.kind: " & $b.ender.kind
            # echo fmt"Cursor: {cursor} values: {values}"
            echo "    ".repeat(cursor) & "   !"
            var x = 0
            for i, v in values:
                if v != 0:
                    x = i
            for i in 0 .. x:
                stdout.write(fmt"{values[i]:4}")
            echo ""
            block_index += 1

proc mmap(address: pointer, length: int, prot: int, flags: int, fd: int, offset: int): ptr uint8 {.importc, header: "sys/mman.h".}
proc mprotect(address: pointer, length: int, prot: int): int {.importc, header: "sys/mman.h".}

var globalInput: string
var globalInputIndex: int
proc runtimeReadInput(): char {.cdecl.} =
    result = if globalInputIndex < globalInput.len: globalInput[globalInputIndex] else: '\0'
    globalInputIndex += 1

proc runtimeWriteOutput(value: char) {.cdecl.} =
    discard stdout.writeChars([value], 0, 1)
    stdout.flushFile()

proc fixImmediate(machine_code: var seq[uint8], immediate: uint32, offset: uint32) =
    machine_code[offset + 0] += uint8(immediate shr (0 * 8))
    machine_code[offset + 1] += uint8(immediate shr (1 * 8))
    machine_code[offset + 2] += uint8(immediate shr (2 * 8))
    machine_code[offset + 3] += uint8(immediate shr (3 * 8))

proc compileAndMap(blocks: seq[AdditionAndMoveBlock]): proc(): int {.cdecl.} =
    const MAX_CODE_SIZE = 4096 * 8
    const MAX_DATA_SIZE = 4096 * 8
    let code_block = (ptr uint8) mmap(nil, MAX_CODE_SIZE, 0, 0x22, -1, 0)
    let data_block = (ptr uint8) mmap(nil, MAX_DATA_SIZE, 3, 0x22, -1, 0)

    var machine_code: seq[uint8] = @[0x90u8,]
    machine_code.add(@[0x53u8]) ## push ebx

    var offsets: seq[uint32]
    var offsets_to_fix: seq[tuple[machine_code_offset: uint32, block_index: int]]
    var immediates_to_fix: seq[tuple[machine_code_offset: uint32, immediate: uint32]]

    machine_code.add(@[0xbbu8, 0, 0, 0, 0,]) ## mov ebx
    immediates_to_fix.add((uint32(machine_code.len - 4), cast[uint32](data_block)))

    # ## Debug print
    # for i, b in blocks:
    #     echo fmt"Block #{i}: increments = {b.increments}, ender = {b.ender.kind}"

    for b in blocks:
        offsets.add(uint32(machine_code.len))
        for offset, value in b.increments:
            let value_to_add = if value < 0: 256 + value else: value
            if offset == 0:
                machine_code.add(@[0x80u8, 0x03, 0]) ## add BYTE PTR [ebx], XXX
                immediates_to_fix.add((uint32(machine_code.len - 1), uint32(value_to_add)))
            elif offset in -128..127:
                machine_code.add(@[0x80u8, 0x43, uint8(offset), 0]) ## add BYTE PTR [ebx + offset], XXX
                immediates_to_fix.add((uint32(machine_code.len - 1), uint32(value_to_add)))
            else:
                dd offset
                assert false, "TODO"
        if b.total_to_the_right != 0:
            if b.total_to_the_right in -128..127:
                machine_code.add(@[0x83u8, 0xc3, uint8(b.total_to_the_right)])
            else:
                machine_code.add(@[0x81u8, 0xc3, 0, 0, 0, 0])
                immediates_to_fix.add((uint32(machine_code.len - 4), uint32(b.total_to_the_right)))
                # assert false, "TODO"
            # ## Check we don't go out of bounds
            # machine_code.add(@[
            #     0x81u8, 0xfb, 0, 0, 0, 0, ## cmp ebx, XXX
            # ])
            # machine_code.add(@[
            #     0x0fu8, 0x8c, 0, 0, 0, 0, ## jl XXX
            # ])
        case b.ender.kind
        of CrashIfZero:
            assert false, "TODO"
        of CrashIfNonZero:
            assert false, "TODO"
        of JumpIfZero:
            machine_code.add(@[
                0x80u8, 0x3b, 0x00,     ## cmp    BYTE PTR [ebx],0x0
                0x0f, 0x84, 0, 0, 0, 0, ## je 00000000
            ])
            offsets_to_fix.add((uint32(machine_code.len - 4), b.ender.jump_block_index))
        of JumpIfNonZero:
            machine_code.add(@[
                0x80u8, 0x3b, 0x00,     ## cmp    BYTE PTR [ebx],0x0
                0x0f, 0x85, 0, 0, 0, 0, ## jne 00000000
            ])
            offsets_to_fix.add((uint32(machine_code.len - 4), b.ender.jump_block_index))
        of WriteOutput:
            machine_code.add(@[
                0x8bu8, 0x03, ## mov eax, [ebx]
                0x50, ## push eax
                0xe8, 0, 0, 0, 0, ## call
            ])
            let call_offset = cast[uint32](runtimeWriteOutput) - (cast[uint32](code_block) + uint32(machine_code.len))
            immediates_to_fix.add((uint32(machine_code.len - 4), call_offset))
            machine_code.add(0x58) ## pop eax
            
        of ReadInput:
            machine_code.add(@[
                0xe8u8, 0, 0, 0, 0, ## call
            ])
            let call_offset = cast[uint32](runtimeReadInput) - (cast[uint32](code_block) + uint32(machine_code.len))
            immediates_to_fix.add((uint32(machine_code.len - 4), call_offset))
            machine_code.add(@[
                0x88u8, 0x03, ## mov BYTE PTR [ebx],al
            ])
        of NothingSpecial:
            discard
        of BEDebugPrint:
            assert false, "TODO"
            
    machine_code.add(@[0x31u8, 0xc0]) ## xor eax, eax
    machine_code.add(@[0x5bu8]) ## pop ebx
    # machine_code.add(0xccu8) ## interrupt
    machine_code.add(0xc3u8) ## ret

    assert machine_code.len <= MAX_CODE_SIZE

    for offset_to_fix in offsets_to_fix:
        fixImmediate(machine_code, offsets[offset_to_fix.block_index] - (offset_to_fix.machine_code_offset + 4u32), offset_to_fix.machine_code_offset)
    for immediate_to_fix in immediates_to_fix:
        fixImmediate(machine_code, immediate=immediate_to_fix.immediate, offset=immediate_to_fix.machine_code_offset)

    var debug_machine_code_string = "Machine code: "
    for x in machine_code:
        debug_machine_code_string &= toHex(x)
    echo debug_machine_code_string

    assert 0 == mprotect(code_block, MAX_CODE_SIZE, 3) ## Read/write
    copyMem(code_block, machine_code[0].addr, machine_code.len)
    assert 0 == mprotect(code_block, MAX_CODE_SIZE, 4) ## Execute

    echo "Compilation sucessful"
    let p = cast[proc(): int {.cdecl.}](code_block)

    return p

proc runFile(source: string, input: string) =
    var ops: seq[Op]
    var start_index_stack: seq[int]
    for chr in source:
        case chr
        # of 'XXX': ops.add(YYY)
        of '+': ops.add(Increment)
        of '-': ops.add(Decrement)
        of '<': ops.add(MoveLeft)
        of '>': ops.add(MoveRight)
        of ',': ops.add(Input)
        of '.': ops.add(Output)
        of '[':
            start_index_stack.add(ops.len)
            ops.add(StartOfLoop)
        of ']':
            if start_index_stack.len == 0:
                ops.add(InvalidEndOfLoop)
            else:
                discard start_index_stack.pop()
                ops.add(EndOfLoop)
        of '@': ops.add(DebugPrint)
        else:
            discard
    for start in start_index_stack:
        ops[start] = InvalidStartOfLoop

    var blocks: seq[AdditionAndMoveBlock]
    var current_block: seq[Op]

    var block_start_index_stack: seq[int]
    proc addBlock(ender: BlockEnder) =
        blocks.add(AdditionAndMoveBlock(moves: current_block, ender: ender))
        current_block = @[]
    for op in ops:
        case op
        of StartOfLoop:
            block_start_index_stack.add(len(blocks))
            addBlock(BlockEnder(kind: JumpIfZero, jump_block_index: -1))
        of InvalidStartOfLoop:
            addBlock(BlockEnder(kind: CrashIfZero))
        of EndOfLoop:
            let start = block_start_index_stack.pop()
            assert blocks[start].ender.kind == JumpIfZero
            blocks[start].ender.jump_block_index = blocks.len + 1
            addBlock(BlockEnder(kind: JumpIfNonZero, jump_block_index: start + 1))
        of InvalidEndOfLoop:
            addBlock(BlockEnder(kind: CrashIfNonZero))
        of Output:
            addBlock(BlockEnder(kind: WriteOutput))
        of Input:
            addBlock(BlockEnder(kind: ReadInput))
        of DebugPrint:
            addBlock(BlockEnder(kind: BEDebugPrint))
        else:
            current_block.add(op)
    addBlock(BlockEnder(kind: NothingSpecial))

    for i in 0 ..< blocks.len:
        var total_to_the_right: int
        var increments: Table[int, int]
        for op in blocks[i].moves:
            case op
            of Increment:
                increments[total_to_the_right] = increments.getOrDefault(total_to_the_right, 0) + 1
            of Decrement:
                increments[total_to_the_right] = increments.getOrDefault(total_to_the_right, 0) - 1
            of MoveLeft:
                total_to_the_right -= 1
            of MoveRight:
                total_to_the_right += 1
            else:
                discard
        blocks[i].total_to_the_right = total_to_the_right
        blocks[i].increments = increments
        # dd (i, blocks[i])
    
    ## Evaluation
    # evaluate(blocks, input, verbose=false)

    ## Compilation
    let code = compileAndMap(blocks)
    globalInput = input
    discard code()

when isMainModule:
    var input = "Heyo!\n"
    if paramCount() >= 1:
        if paramCount() == 2:
            input = readFile(paramStr(2))
        runFile(readFile(paramStr(1)), input)
    else:
        echo "Enter program:"
        stdout.flushFile()
        let source = readLine(stdin)
        echo "Compiling program"
        stdout.flushFile()
        runFile(source, input)

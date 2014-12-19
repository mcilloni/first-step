"  This file is part of First Step.
"  
"  First Step is free software: you can redistribute it and/or modify it under the terms of the GNU Lesser General Public License as published by the Free Software 
"  Foundation, either version 3 of the License, or (at your option) any later version. 
"
"  First Step is distributed in the hope that it will be useful, but 
"  WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
"  GNU Lesser General Public License for more details.
"
"  You should have received a copy of the GNU Lesser General Public License
"  along with First Step.  If not, see <http://www.gnu.org/licenses/>
"
"  Copyright (C) Marco Cilloni <marco.cilloni@yahoo.com> 2014

" Quit when a (custom) syntax file was already loaded
if exists("b:current_syntax")
  finish
endif

if !exists("fork_highlight_array_whitespace_error")
  let fork_highlight_array_whitespace_error = 1
endif
if !exists("fork_highlight_chan_whitespace_error")
  let fork_highlight_chan_whitespace_error = 1
endif
if !exists("fork_highlight_extra_types")
  let fork_highlight_extra_types = 1
endif
if !exists("fork_highlight_space_tab_error")
  let fork_highlight_space_tab_error = 1
endif
if !exists("fork_highlight_trailing_whitespace_error")
  let fork_highlight_trailing_whitespace_error = 1
endif

syn case match

"syn keyword     forkDirective         module import
syn keyword     forkDeclaration       mut decl alias module import
syn keyword     forkDeclType          struct func entry
syn match       forkDeclType          "\/func" 
syn match       forkDeclType          "\/entry" 

"hi def link     forkDirective         Statement
hi def link     forkDeclaration       Keyword
hi def link     forkDeclType          Keyword

" Keywords within functions
syn keyword     forkStatement         return break continue 
syn keyword     forkConditional       if else 
syn match       forkConditional       "\/if"
"syn keyword     forkLabel             case default
syn keyword     forkRepeat            while
syn match       forkRepeat            "\/while"

hi def link     forkStatement         Statement
hi def link     forkConditional       Conditional
"hi def link     forkLabel             Label
hi def link     forkRepeat            Repeat

" Predefined types
syn keyword     forkType              bool
syn keyword     forkSignedInts        int8 int16 int32 int64 intptr
syn keyword     forkUnsignedInts      uint8 uint16 uint32 uint64 uintptr
syn keyword     forkData              data

hi def link     forkType              Type
hi def link     forkSignedInts        Type
hi def link     forkUnsignedInts      Type
hi def link     forkData              Type

" Predefined functions and values
"syn keyword     forkBuiltins          append cap close complex copy delete imag len
"syn keyword     forkBuiltins          make new panic print println real recover
syn keyword     forkConstants         true false null
syn keyword     forkKey               ptr val or and mod size cast xor

"hi def link     forkBuiltins          Keyword
hi def link     forkConstants         Keyword
hi def link     forkKey               Operator 

" Comments; their contents
syn keyword     forkTodo              contained TODO FIXME XXX BUG
syn cluster     forkCommentGroup      contains=forkTodo
syn region      forkComment           start="#" end="$" contains=@forkCommentGroup,@Spell

hi def link     forkComment           Comment
hi def link     forkTodo              Todo

"syn match       forkEscapeOctal       display contained "\\[0-7]\{3}"
"syn match       forkEscapeC           display contained +\\[abfnrtv\\'"]+
"syn match       forkEscapeX           display contained "\\x\x\{2}"
"syn match       forkEscapeU           display contained "\\u\x\{4}"
"syn match       forkEscapeBigU        display contained "\\U\x\{8}"
"syn match       forkEscapeError       display contained +\\[^0-7xuUabfnrtv\\'"]+

"hi def link     forkEscapeOctal       forkSpecialString
"hi def link     forkEscapeC           forkSpecialString
"hi def link     forkEscapeX           forkSpecialString
"hi def link     forkEscapeU           forkSpecialString
"hi def link     forkEscapeBigU        forkSpecialString
"hi def link     forkSpecialString     Special
"hi def link     forkEscapeError       Error

" Strings and their contents
"syn cluster     forkStringGroup       contains=forkEscapeOctal,forkEscapeC,forkEscapeX,forkEscapeU,forkEscapeBigU,forkEscapeError
syn region      forkString            start=+"+ skip=+\\\\\|\\"+ end=+"+ 
"contains=@forkStringGroup
"syn region      forkRawString         start=+`+ end=+`+

hi def link     forkString            String
"hi def link     forkRawString         String

" Characters; their contents
"syn cluster     forkCharacterGroup    contains=forkEscapeOctal,forkEscapeC,forkEscapeX,forkEscapeU,forkEscapeBigU
"syn region      forkCharacter         start=+'+ skip=+\\\\\|\\'+ end=+'+ contains=@forkCharacterGroup

"hi def link     forkCharacter         Character

" Regions
"syn region      forkBlock             start="{" end="}" transparent fold
syn region      forkParen             start='(' end=')' transparent

" Integers
syn match       forkDecimalInt        "\<\d\+\([Ee]\d\+\)\?\>"
"syn match       forkHexadecimalInt    "\<0x\x\+\>"
"syn match       forkOctalInt          "\<0\o\+\>"
"syn match       forkOctalError        "\<0\o*[89]\d*\>"

hi def link     forkDecimalInt        Integer
"hi def link     forkHexadecimalInt    Integer
"hi def link     forkOctalInt          Integer
hi def link     Integer             Number

" Floating point
"syn match       forkFloat             "\<\d\+\.\d*\([Ee][-+]\d\+\)\?\>"
"syn match       forkFloat             "\<\.\d\+\([Ee][-+]\d\+\)\?\>"
"syn match       forkFloat             "\<\d\+[Ee][-+]\d\+\>"

"hi def link     forkFloat             Float

" Imaginary literals
"syn match       forkImaginary         "\<\d\+i\>"
"syn match       forkImaginary         "\<\d\+\.\d*\([Ee][-+]\d\+\)\?i\>"
"syn match       forkImaginary         "\<\.\d\+\([Ee][-+]\d\+\)\?i\>"
"syn match       forkImaginary         "\<\d\+[Ee][-+]\d\+i\>"

"hi def link     forkImaginary         Number

" Spaces after "[]"
if fork_highlight_array_whitespace_error != 0
  syn match forkSpaceError display "\(\[\]\)\@<=\s\+"
endif

" Space-tab error
if fork_highlight_space_tab_error != 0
  syn match forkSpaceError display " \+\t"me=e-1
endif

" Trailing white space error
if fork_highlight_trailing_whitespace_error != 0
  syn match forkSpaceError display excludenl "\s\+$"
endif

hi def link     forkExtraType         Type
hi def link     forkSpaceError        Error

" Search backwards for a global declaration to start processing the syntax.
"syn sync match forkSync grouphere NONE /^\(const\|var\|type\|func\)\>/

" There's a bug in the implementation of grouphere. For now, use the
" following as a more expensive/less precise workaround.
syn sync minlines=500

let b:current_syntax = "fork"

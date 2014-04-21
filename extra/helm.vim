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

if !exists("helm_highlight_array_whitespace_error")
  let helm_highlight_array_whitespace_error = 1
endif
if !exists("helm_highlight_chan_whitespace_error")
  let helm_highlight_chan_whitespace_error = 1
endif
if !exists("helm_highlight_extra_types")
  let helm_highlight_extra_types = 1
endif
if !exists("helm_highlight_space_tab_error")
  let helm_highlight_space_tab_error = 1
endif
if !exists("helm_highlight_trailing_whitespace_error")
  let helm_highlight_trailing_whitespace_error = 1
endif

syn case match

"syn keyword     helmDirective         module import
syn keyword     helmDeclaration       var decl alias
syn keyword     helmDeclType          struct func entry
syn match       helmDeclType          "\/func" 
syn match       helmDeclType          "\/entry" 

"hi def link     helmDirective         Statement
hi def link     helmDeclaration       Keyword
hi def link     helmDeclType          Keyword

" Keywords within functions
syn keyword     helmStatement         return 
syn keyword     helmConditional       if else 
syn match       helmConditional       "\/if"
"syn keyword     helmLabel             case default
syn keyword     helmRepeat            while
syn match       helmRepeat            "\/while"

hi def link     helmStatement         Statement
hi def link     helmConditional       Conditional
"hi def link     helmLabel             Label
hi def link     helmRepeat            Repeat

" Predefined types
syn keyword     helmType              bool
syn keyword     helmSignedInts        int8 int16 int32 int64
syn keyword     helmUnsignedInts      uint8 uint16 uint32 uint64 uintptr
syn keyword     helmData              data

hi def link     helmType              Type
hi def link     helmSignedInts        Type
hi def link     helmUnsignedInts      Type
hi def link     helmData              Type

" Predefined functions and values
"syn keyword     helmBuiltins          append cap close complex copy delete imag len
"syn keyword     helmBuiltins          make new panic print println real recover
syn keyword     helmConstants         true false null
syn keyword     helmKey               ptr val or and size cast

"hi def link     helmBuiltins          Keyword
hi def link     helmConstants         Keyword
hi def link     helmKey               Keyword

" Comments; their contents
syn keyword     helmTodo              contained TODO FIXME XXX BUG
syn cluster     helmCommentGroup      contains=helmTodo
syn region      helmComment           start="#" end="$" contains=@helmCommentGroup,@Spell

hi def link     helmComment           Comment
hi def link     helmTodo              Todo

"syn match       helmEscapeOctal       display contained "\\[0-7]\{3}"
"syn match       helmEscapeC           display contained +\\[abfnrtv\\'"]+
"syn match       helmEscapeX           display contained "\\x\x\{2}"
"syn match       helmEscapeU           display contained "\\u\x\{4}"
"syn match       helmEscapeBigU        display contained "\\U\x\{8}"
"syn match       helmEscapeError       display contained +\\[^0-7xuUabfnrtv\\'"]+

"hi def link     helmEscapeOctal       helmSpecialString
"hi def link     helmEscapeC           helmSpecialString
"hi def link     helmEscapeX           helmSpecialString
"hi def link     helmEscapeU           helmSpecialString
"hi def link     helmEscapeBigU        helmSpecialString
"hi def link     helmSpecialString     Special
"hi def link     helmEscapeError       Error

" Strings and their contents
"syn cluster     helmStringGroup       contains=helmEscapeOctal,helmEscapeC,helmEscapeX,helmEscapeU,helmEscapeBigU,helmEscapeError
syn region      helmString            start=+"+ skip=+\\\\\|\\"+ end=+"+ 
"contains=@helmStringGroup
"syn region      helmRawString         start=+`+ end=+`+

hi def link     helmString            String
"hi def link     helmRawString         String

" Characters; their contents
"syn cluster     helmCharacterGroup    contains=helmEscapeOctal,helmEscapeC,helmEscapeX,helmEscapeU,helmEscapeBigU
"syn region      helmCharacter         start=+'+ skip=+\\\\\|\\'+ end=+'+ contains=@helmCharacterGroup

"hi def link     helmCharacter         Character

" Regions
"syn region      helmBlock             start="{" end="}" transparent fold
syn region      helmParen             start='(' end=')' transparent

" Integers
syn match       helmDecimalInt        "\<\d\+\([Ee]\d\+\)\?\>"
"syn match       helmHexadecimalInt    "\<0x\x\+\>"
"syn match       helmOctalInt          "\<0\o\+\>"
"syn match       helmOctalError        "\<0\o*[89]\d*\>"

hi def link     helmDecimalInt        Integer
"hi def link     helmHexadecimalInt    Integer
"hi def link     helmOctalInt          Integer
hi def link     Integer             Number

" Floating point
"syn match       helmFloat             "\<\d\+\.\d*\([Ee][-+]\d\+\)\?\>"
"syn match       helmFloat             "\<\.\d\+\([Ee][-+]\d\+\)\?\>"
"syn match       helmFloat             "\<\d\+[Ee][-+]\d\+\>"

"hi def link     helmFloat             Float

" Imaginary literals
"syn match       helmImaginary         "\<\d\+i\>"
"syn match       helmImaginary         "\<\d\+\.\d*\([Ee][-+]\d\+\)\?i\>"
"syn match       helmImaginary         "\<\.\d\+\([Ee][-+]\d\+\)\?i\>"
"syn match       helmImaginary         "\<\d\+[Ee][-+]\d\+i\>"

"hi def link     helmImaginary         Number

" Spaces after "[]"
if helm_highlight_array_whitespace_error != 0
  syn match helmSpaceError display "\(\[\]\)\@<=\s\+"
endif

" Space-tab error
if helm_highlight_space_tab_error != 0
  syn match helmSpaceError display " \+\t"me=e-1
endif

" Trailing white space error
if helm_highlight_trailing_whitespace_error != 0
  syn match helmSpaceError display excludenl "\s\+$"
endif

hi def link     helmExtraType         Type
hi def link     helmSpaceError        Error

" Search backwards for a global declaration to start processing the syntax.
"syn sync match helmSync grouphere NONE /^\(const\|var\|type\|func\)\>/

" There's a bug in the implementation of grouphere. For now, use the
" following as a more expensive/less precise workaround.
syn sync minlines=500

let b:current_syntax = "helm"

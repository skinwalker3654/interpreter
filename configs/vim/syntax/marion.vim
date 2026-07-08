" Vim syntax file
" Language: Crazy
" Extension: .crazy

if exists("b:current_syntax")
  finish
endif


" ====================
" Comments
" ====================

syntax region crazyComment start="#" end="$"
syntax keyword crazyTodo TODO FIXME NOTE contained


" ====================
" Keywords
" ====================

syntax keyword crazyKeyword
      \ metablhth
      \ set
      \ if
      \ an
      \ for
      \ to
      \ gia
      \ eos
      \ endprogram
      \ telosprograma


" ====================
" Commands
" ====================

syntax keyword crazyCommand
      \ treje
      \ execute
      \ remove
      \ sbyse
      \ perimene
      \ wait
      \ clear
      \ ka8arise


" ====================
" Built-in functions
" ====================

syntax keyword crazyBuiltin
      \ print
      \ printent
      \ println
      \ diabase
      \ read
      \ read_file
      \ mkdir
      \ diabase_arxeio
      \ neosfakelos


syntax keyword crazytype
      \ ari8mo
      \ mhnhma
      \ int
      \ str
      \ len
      \ mhkos

" ====================
" Numbers
" ====================

syntax match crazyNumber "\<\d\+\>"


" ====================
" Strings
" ====================

syntax region crazyString start='"' end='"'


" ====================
" Operators
" ====================

syntax match crazyOperator "[+\-*/><=]"


" ====================
" Brackets
" ====================

syntax match crazyBracket "[{}()]"


" ====================
" Variables
" ====================

syntax match crazyVariable "\<[a-zA-Z_][a-zA-Z0-9_]*\>"


" ====================
" Functions
" ====================

syntax match crazyFunction "\<[a-zA-Z_][a-zA-Z0-9_]*\ze("

highlight crazyComment  guifg=#808080 gui=italic
highlight crazyTodo     guifg=#FFB000 gui=bold
highlight crazyKeyword  guifg=#CC7832 gui=bold
highlight crazyCommand  guifg=#BBB529 gui=bold
highlight crazyBuiltin  guifg=#FFC66D gui=bold
highlight crazyType     guifg=#87CEEB gui=bold
highlight crazyString   guifg=#6A8759
highlight crazyNumber   guifg=#6897BB
highlight crazyOperator guifg=#A9B7C6 gui=bold
highlight crazyBracket  guifg=#A9B7C6
highlight crazyVariable guifg=#B9C0CB
highlight crazyFunction guifg=#FFC66D gui=bold

let b:current_syntax = "crazy"

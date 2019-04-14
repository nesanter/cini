" Vim syntax file
" Language: "Simple Safe Settings" INI
" Maintainer: Noah Santer
" Latest Revision: 25 Feb 2019

" A modeline at the end of the INI file
" can be used to enable this syntax per-file:
" vim: set syn=sssini:

syn match sssiniRemoveKey "^\s*\zs[^=]\{-1,}\ze\s*$"

syn match sssiniAssign "^.\{-}=.*$" contains=sssiniKey
syn match sssiniKey "^\zs[^=]*\ze=" contained

syn match sssiniSection "^\s*\zs\[[^\]]*\]\ze\s*$" contains=sssiniSectionName
syn match sssiniSectionName "\[\zs[^\]]*\ze\]" contained

syn match sssiniComment "^\s*#.*$" contains=sssiniPound
syn match sssiniPound "#" contained

hi sssiniComment ctermfg=4
hi sssiniPound ctermfg=4

" hi sssiniSection ...
hi sssiniSectionName ctermfg=1

" hi sssiniAssign cterm=italic

" hi sssiniKey ctermfg=3
" hi sssiniRemoveKey ctermfg=3 cterm=strikethrough
hi sssiniKey cterm=bold
hi sssiniRemoveKey cterm=strikethrough,bold

let b:current_syntax = "sssini"

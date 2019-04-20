# cini

`./configure release && ninja`

## Syntax

1. strip whitespace (left and right)
2. if empty, `INI_EVENT_NONE`
3. if first character is `#`, `INI_EVENT_COMMENT`
4. if first character `[` and last `]`, `INI_EVENT_SECTION`
5. if contains `=`,
   split key and value at first occurance, `INI_EVENT_KEY_VALUE`
6. else, `INI_EVENT_KEY_ONLY`

The assumed interpretation of a `KEY_ONLY` event is as a key removal.


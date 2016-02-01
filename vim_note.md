Working with VIM

touch fileName.xxx
vim fileName.xxx		or just: vi fileName.xxx

i - insert
(esc) :w! 		: force write
(esc) :q! 		: force quit
(esc) :wq! 		: force write then quit
dd				: delete line
yy				: copy line
yy then p		: copy line then paste to line after
yy then Shift p	: copy line then paste to line before
(esc):u			: undo latest changes
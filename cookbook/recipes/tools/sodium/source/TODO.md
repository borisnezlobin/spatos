- [x] Make editor.pos method and use that instead of
- [ ] Add word navigation
- [ ] `.` command
- [ ] More partial redrawing (register "is_modified")


Known bugs:

- [x] When using `t` with a char that isn't in the document, Sodium will crash.
- [x] `dG` on the last line of the file deletes from the cursor to the end of the line, instead of the entire line.
      Not sure if intended.

The bug causing these two bugs, is localised to be in position.rs. It resolves by returning a value one over bound x

- [x] The x value is wrongly bounded. Reproduction:
      1) Make two lines:
         - abc
         - abcdef
      2) Go to the end of the first line.
      3) Go one down. As you'll see you'll end up at d. That's right.
      4) Now go two the end of the first line again.
      5) Type 2l.
      6) Now go one down
      7) You'll end up on e, even though it should be d

- [x] Crashes when:
      1) Write abc on line 1
      2) Press o to go to the next line
      3) Go to normal mode
      4) Press a and go to append mode
      5) Type text
      6) Out of bound (index) error

- [x] When typing the first char in a line in normal insert mode, it wont go to the next char.

- [x] The modifier keys are only working for one command
     Solutions:
     - Make a struct KeyState storing info on the modifiers active. Add a method `feed` which feeds the keystate with a key, updating it. This should Option<Key>, where a key should be returned iff the key entered was not a modifier

- [ ] Crashes when ~ command is used on an empty line
- [ ] `z` command is buggy.
- [ ] `x` is buggy (when line length differ)

Refactoring:
- Organize into modules

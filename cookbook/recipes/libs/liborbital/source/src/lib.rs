extern crate orbclient;

use std::ffi::CStr;
use std::mem::transmute;
use std::os::raw::c_char;

use orbclient::{Window, WindowFlag, Renderer, EventIter, get_display_size};
use orbclient::event::*;

// Should be in sync with `orbclient::WindowFlag`
pub const ORB_WINDOW_ASYNC: u32 = 0x0001;
pub const ORB_WINDOW_BACK: u32 = 0x0002;
pub const ORB_WINDOW_FRONT: u32 = 0x0004;
pub const ORB_WINDOW_RESIZABLE: u32 = 0x0008;
pub const ORB_WINDOW_UNCLOSABLE: u32 = 0x0010;

// Should be in sync with `orbclient::event::K_*`
pub const ORB_KEY_TICK: u8 = 0x29;
pub const ORB_KEY_MINUS: u8 = 0x0C;
pub const ORB_KEY_EQUALS: u8 = 0x0D;
pub const ORB_KEY_BACKSLASH: u8 = 0x2B;
pub const ORB_KEY_BRACE_OPEN: u8 = 0x1A;
pub const ORB_KEY_BRACE_CLOSE: u8 = 0x1B;
pub const ORB_KEY_SEMICOLON: u8 = 0x27;
pub const ORB_KEY_QUOTE: u8 = 0x28;
pub const ORB_KEY_COMMA: u8 = 0x33;
pub const ORB_KEY_PERIOD: u8 = 0x34;
pub const ORB_KEY_SLASH: u8 = 0x35;
pub const ORB_KEY_BKSP: u8 = 0x0E;
pub const ORB_KEY_SPACE: u8 = 0x39;
pub const ORB_KEY_TAB: u8 = 0x0F;
pub const ORB_KEY_CAPS: u8 = 0x3A;
pub const ORB_KEY_LEFT_SHIFT: u8 = 0x2A;
pub const ORB_KEY_RIGHT_SHIFT: u8 = 0x36;
pub const ORB_KEY_CTRL: u8 = 0x1D;
pub const ORB_KEY_ALT: u8 = 0x38;
pub const ORB_KEY_ENTER: u8 = 0x1C;
pub const ORB_KEY_ESC: u8 = 0x01;
pub const ORB_KEY_F1: u8 = 0x3B;
pub const ORB_KEY_F2: u8 = 0x3C;
pub const ORB_KEY_F3: u8 = 0x3D;
pub const ORB_KEY_F4: u8 = 0x3E;
pub const ORB_KEY_F5: u8 = 0x3F;
pub const ORB_KEY_F6: u8 = 0x40;
pub const ORB_KEY_F7: u8 = 0x41;
pub const ORB_KEY_F8: u8 = 0x42;
pub const ORB_KEY_F9: u8 = 0x43;
pub const ORB_KEY_F10: u8 = 0x44;
pub const ORB_KEY_HOME: u8 = 0x47;
pub const ORB_KEY_UP: u8 = 0x48;
pub const ORB_KEY_PGUP: u8 = 0x49;
pub const ORB_KEY_LEFT: u8 = 0x4B;
pub const ORB_KEY_RIGHT: u8 = 0x4D;
pub const ORB_KEY_END: u8 = 0x4F;
pub const ORB_KEY_DOWN: u8 = 0x50;
pub const ORB_KEY_PGDN: u8 = 0x51;
pub const ORB_KEY_DEL: u8 = 0x53;
pub const ORB_KEY_F11: u8 = 0x57;
pub const ORB_KEY_F12: u8 = 0x58;

// These scancodes are missing from `orbclient`
pub const ORB_KEY_INSERT: u8 = 0x52;
pub const ORB_KEY_SCROLL: u8 = 0x46;
pub const ORB_KEY_0: u8 = 0x0B;
pub const ORB_KEY_1: u8 = 0x02;
pub const ORB_KEY_2: u8 = 0x03;
pub const ORB_KEY_3: u8 = 0x04;
pub const ORB_KEY_4: u8 = 0x05;
pub const ORB_KEY_5: u8 = 0x06;
pub const ORB_KEY_6: u8 = 0x07;
pub const ORB_KEY_7: u8 = 0x08;
pub const ORB_KEY_8: u8 = 0x09;
pub const ORB_KEY_9: u8 = 0x0A;
pub const ORB_KEY_A: u8 = 0x1E;
pub const ORB_KEY_B: u8 = 0x30;
pub const ORB_KEY_C: u8 = 0x2E;
pub const ORB_KEY_D: u8 = 0x20;
pub const ORB_KEY_E: u8 = 0x12;
pub const ORB_KEY_F: u8 = 0x21;
pub const ORB_KEY_G: u8 = 0x22;
pub const ORB_KEY_H: u8 = 0x23;
pub const ORB_KEY_I: u8 = 0x17;
pub const ORB_KEY_J: u8 = 0x24;
pub const ORB_KEY_K: u8 = 0x25;
pub const ORB_KEY_L: u8 = 0x26;
pub const ORB_KEY_M: u8 = 0x32;
pub const ORB_KEY_N: u8 = 0x31;
pub const ORB_KEY_O: u8 = 0x18;
pub const ORB_KEY_P: u8 = 0x19;
pub const ORB_KEY_Q: u8 = 0x10;
pub const ORB_KEY_R: u8 = 0x13;
pub const ORB_KEY_S: u8 = 0x1F;
pub const ORB_KEY_T: u8 = 0x14;
pub const ORB_KEY_U: u8 = 0x16;
pub const ORB_KEY_V: u8 = 0x2F;
pub const ORB_KEY_W: u8 = 0x11;
pub const ORB_KEY_X: u8 = 0x2D;
pub const ORB_KEY_Y: u8 = 0x15;
pub const ORB_KEY_Z: u8 = 0x2C;

// Should be in sync with `orbclient::event::EventOption`
#[repr(C)]
pub enum OrbEventOption {
    Key {
        character: u32,
        scancode: u8,
        pressed: bool,
    },
    TextInput {
        character: u32,
    },
    Mouse {
        x: i32,
        y: i32,
    },
    MouseRelative {
        dx: i32,
        dy: i32,
    },
    Button {
        left: bool,
        middle: bool,
        right: bool,
    },
    Scroll {
        x: i32,
        y: i32,
    },
    Quit {
    },
    Focus {
        focused: bool,
    },
    Move {
        x: i32,
        y: i32,
    },
    Resize {
        width: u32,
        height: u32,
    },
    Screen {
        width: u32,
        height: u32,
    },
    Clipboard {
        kind: u8,
        size: usize,
    },
    ClipboardUpdate,
    Drop {
        kind: u8,
    },
    Hover {
        entered: bool,
    },
    Unknown {
        code: i64,
        a: i64,
        b: i64,
    },
    None,
}

impl From<EventOption> for OrbEventOption {
    fn from(event: EventOption) -> Self {
        match event {
            EventOption::Key(KeyEvent { character, scancode, pressed }) => {
                OrbEventOption::Key { character: character as u32, scancode, pressed }
            },
            EventOption::TextInput(TextInputEvent { character }) => {
                OrbEventOption::TextInput { character: character as u32 }
            },
            EventOption::Mouse(MouseEvent { x, y }) => {
                OrbEventOption::Mouse { x, y }
            },
            EventOption::MouseRelative(MouseRelativeEvent { dx, dy }) => {
                OrbEventOption::MouseRelative { dx, dy }
            },
            EventOption::Button(ButtonEvent { left, middle, right }) => {
                OrbEventOption::Button { left, middle, right }
            },
            EventOption::Scroll(ScrollEvent { x, y }) => {
                OrbEventOption::Scroll { x, y }
            },
            EventOption::Quit(QuitEvent { }) => {
                OrbEventOption::Quit { }
            },
            EventOption::Focus(FocusEvent { focused }) => {
                OrbEventOption::Focus { focused }
            },
            EventOption::Move(MoveEvent { x, y }) => {
                OrbEventOption::Move { x, y }
            },
            EventOption::Resize(ResizeEvent { width, height }) => {
                OrbEventOption::Resize { width, height }
            },
            EventOption::Screen(ScreenEvent { width, height }) => {
                OrbEventOption::Screen { width, height }
            },
            EventOption::Clipboard(ClipboardEvent { kind, size }) => {
                OrbEventOption::Clipboard { kind, size }
            },
            EventOption::ClipboardUpdate(ClipboardUpdateEvent) => {
                OrbEventOption::ClipboardUpdate
            },
            EventOption::Drop(DropEvent { kind }) => {
                OrbEventOption::Drop { kind }
            },
            EventOption::Hover(HoverEvent { entered }) => {
                OrbEventOption::Hover { entered }
            },
            EventOption::Unknown(Event { code, a, b }) => {
                OrbEventOption::Unknown { code, a, b }
            },
            EventOption::None => {
                OrbEventOption::None
            },
        }
    }
}

#[no_mangle]
pub unsafe extern "C" fn orb_display_width() -> u32 {
    get_display_size().unwrap().0
}

#[no_mangle]
pub unsafe extern "C" fn orb_display_height() -> u32 {
    get_display_size().unwrap().1
}

#[no_mangle]
pub unsafe extern "C" fn orb_window_new(
    x: i32,
    y: i32,
    w: u32,
    h: u32,
    title: *const c_char,
) -> *mut Window {
    orb_window_new_flags(x, y, w, h, title, 0)
}

#[no_mangle]
pub unsafe extern "C" fn orb_window_new_flags(
    x: i32,
    y: i32,
    w: u32,
    h: u32,
    title: *const c_char,
    flags: u32,
) -> *mut Window {
    let flags_vec = {
        let mut res = Vec::new();
        if flags & ORB_WINDOW_ASYNC != 0 { res.push(WindowFlag::Async); }
        if flags & ORB_WINDOW_BACK != 0 { res.push(WindowFlag::Back); }
        if flags & ORB_WINDOW_FRONT != 0 { res.push(WindowFlag::Front); }
        if flags & ORB_WINDOW_RESIZABLE != 0 { res.push(WindowFlag::Resizable); }
        if flags & ORB_WINDOW_UNCLOSABLE != 0 { res.push(WindowFlag::Unclosable); }
        res
    };

    let title = CStr::from_ptr(title).to_string_lossy();
    transmute(Box::new(Window::new_flags(x, y, w, h, &title, &flags_vec).unwrap()))
}

#[no_mangle]
pub unsafe extern "C" fn orb_window_destroy(window: &mut Window) {
    let window: Box<Window> = transmute(window);
    drop(window);
}

#[no_mangle]
pub unsafe extern "C" fn orb_window_data(window: &mut Window) ->  *mut u32 {
    window.data_mut().as_mut_ptr() as *mut u32
}

#[no_mangle]
pub unsafe extern "C" fn orb_window_width(window: &mut Window) -> u32 {
    window.width()
}

#[no_mangle]
pub unsafe extern "C" fn orb_window_height(window: &mut Window) -> u32 {
    window.height()
}

#[no_mangle]
pub unsafe extern "C" fn orb_window_x(window: &mut Window) -> i32 {
    window.x()
}

#[no_mangle]
pub unsafe extern "C" fn orb_window_y(window: &mut Window) -> i32 {
    window.y()
}

#[no_mangle]
pub unsafe extern "C" fn orb_window_set_mouse_cursor(window: &mut Window, cursor: bool) {
    window.set_mouse_cursor(cursor);
}

#[no_mangle]
pub unsafe extern "C" fn orb_window_set_mouse_grab(window: &mut Window, grab: bool) {
    window.set_mouse_grab(grab);
}

#[no_mangle]
pub unsafe extern "C" fn orb_window_set_mouse_relative(window: &mut Window, relative: bool) {
    window.set_mouse_relative(relative);
}

#[no_mangle]
pub unsafe extern "C" fn orb_window_set_pos(window: &mut Window, x: i32, y: i32) {
    window.set_pos(x, y);
}

#[no_mangle]
pub unsafe extern "C" fn orb_window_set_size(window: &mut Window, w: u32, h: u32) {
    window.set_size(w, h);
}

#[no_mangle]
pub unsafe extern "C" fn orb_window_set_title(window: &mut Window, title: *const c_char) {
    let title = CStr::from_ptr(title).to_string_lossy();
    window.set_title(&title);
}

#[no_mangle]
pub unsafe extern "C" fn orb_window_sync(window: &mut Window) {
    window.sync();
}

#[no_mangle]
pub unsafe extern "C" fn orb_window_events(window: &mut Window) -> *mut EventIter {
    transmute(Box::new(window.events()))
}

#[no_mangle]
pub unsafe extern "C" fn orb_events_next(event_iter: &mut EventIter) -> OrbEventOption {
    if let Some(event) = event_iter.next() {
        OrbEventOption::from(event.to_option())
    } else {
        OrbEventOption::None
    }
}

#[no_mangle]
pub unsafe extern "C" fn orb_events_destroy(event_iter: &mut EventIter) {
    let event_iter: Box<EventIter> = transmute(event_iter);
    drop(event_iter);
}

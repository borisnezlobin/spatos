use std::collections::{BTreeMap, VecDeque};
use std::str;
use syscall::error::{EBADF, EINVAL, ENOENT, EWOULDBLOCK, Error, Result};
use syscall::flag::O_NONBLOCK;

use redox_scheme::SchemeBlockMut;

// The strict buffer size of the audiohw: driver
const HW_BUFFER_SIZE: usize = 512;
// The desired buffer size of each handle
const HANDLE_BUFFER_SIZE: usize = 4096;

enum Handle {
    Audio {
        flags: usize,
        buffer: VecDeque<(i16, i16)>,
    },
    //TODO: move volume to audiohw:?
    Volume {
        flags: usize,
        offset: usize,
    }
}

pub struct AudioScheme {
    next_id: usize,
    handles: BTreeMap<usize, Handle>,
    volume: i32,
}

impl AudioScheme {
    pub fn new() -> Self {
        AudioScheme {
            next_id: 0,
            handles: BTreeMap::new(),
            volume: 50,
        }
    }

    pub fn buffer(&mut self) -> [(i16, i16); HW_BUFFER_SIZE] {
        let mut mix_buffer = [(0i16, 0i16); HW_BUFFER_SIZE];

        // Multiply each sample by the cube of volume divided by 100
        // This mimics natural perception of loudness
        let volume_factor = ((self.volume as f32) / 100.0).powi(3);
        for (_id, handle) in self.handles.iter_mut() {
            match handle {
                Handle::Audio { flags: _, ref mut buffer } => {
                    let mut i = 0;
                    while i < mix_buffer.len() {
                        if let Some(sample) = buffer.pop_front() {
                            let left = (sample.0 as f32 * volume_factor) as i16;
                            let right = (sample.1 as f32 * volume_factor) as i16;
                            mix_buffer[i].0 = mix_buffer[i].0.saturating_add(left);
                            mix_buffer[i].1 = mix_buffer[i].1.saturating_add(right);
                        } else {
                            break;
                        }
                        i += 1;
                    }
                },
                _ => (),
            }
        }

        mix_buffer
    }
}

impl SchemeBlockMut for AudioScheme {
    fn open(&mut self, path: &str, flags: usize, _uid: u32, _gid: u32) -> Result<Option<usize>> {
        let handle = match path.trim_matches('/') {
            "" => Handle::Audio {
                flags,
                buffer: VecDeque::new()
            },
            "volume" => Handle::Volume {
                flags,
                offset: 0,
            },
            _ => return Err(Error::new(ENOENT)),
        };

        let id = self.next_id;
        self.next_id += 1;
        self.handles.insert(id, handle);

        Ok(Some(id))
    }

    fn read(&mut self, id: usize, buf: &mut [u8]) -> Result<Option<usize>> {
        //TODO: check flags for readable
        match self.handles.get_mut(&id).ok_or(Error::new(EBADF))? {
            Handle::Audio { flags: _, buffer: _ } => {
                //TODO: audio input?
                Err(Error::new(EBADF))
            },
            Handle::Volume { flags: _, ref mut offset } => {
                //TODO: should we allocate every time?
                let string = format!("{}", self.volume);
                let bytes = string.as_bytes();

                let mut i = 0;
                while i < buf.len() && *offset + i < bytes.len() {
                    buf[i] = bytes[*offset + i];
                    i += 1;
                }

                *offset += i;
                Ok(Some(i))
            }
        }
    }

    fn write(&mut self, id: usize, buf: &[u8]) -> Result<Option<usize>> {
        //TODO: check flags for writable
        match self.handles.get_mut(&id).ok_or(Error::new(EBADF))? {
            Handle::Audio { ref flags, ref mut buffer } => {
                if buffer.len() >= HANDLE_BUFFER_SIZE {
                    if flags & O_NONBLOCK > 0 {
                        Err(Error::new(EWOULDBLOCK))
                    } else {
                        Ok(None)
                    }
                } else {
                    let mut i = 0;
                    while i + 4 <= buf.len() {
                        buffer.push_back((
                            (buf[i] as i16) | ((buf[i + 1] as i16) << 8),
                            (buf[i + 2] as i16) | ((buf[i + 3] as i16) << 8)
                        ));

                        i += 4;
                    }

                    Ok(Some(i))
                }
            },
            Handle::Volume { flags: _, ref mut offset } => {
                //TODO: support other offsets?
                if *offset == 0 {
                    let value = str::from_utf8(buf)
                        .map_err(|_| Error::new(EINVAL))?
                        .trim()
                        .parse::<i32>()
                        .map_err(|_| Error::new(EINVAL))?;
                    if value >= 0 && value <= 100 {
                        self.volume = value;
                        *offset += buf.len();
                        Ok(Some(buf.len()))
                    } else {
                        Err(Error::new(EINVAL))
                    }
                } else {
                    // EOF
                    Ok(Some(0))
                }
            }
        }
    }
}

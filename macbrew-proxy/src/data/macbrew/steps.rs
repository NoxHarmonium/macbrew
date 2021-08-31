use serde::Serialize;

pub const PREPARE_PHASE: u8 = 16;
pub const MASH_PHASE: u8 = 18;
pub const BOIL_PHASE: u8 = 20;
pub const CHILL_PHASE: u8 = 22;
pub const CLEANUP_PHASE: u8 = 24;

#[repr(C)]
#[derive(Serialize, Debug, Clone)]
pub struct BrewSessionStep {
    pub description: String,
    pub time: i16,
    pub phase: u8,
}

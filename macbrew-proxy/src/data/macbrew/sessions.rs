use crate::data::brewers_friend::sessions::BfBrewSession;
use crate::data::brewers_friend::sessions::BfBrewSessionFull;
use serde::{Deserialize, Serialize};

/// Enough information to decide which session to fetch more info on
#[repr(C)]
#[derive(Serialize, Debug, Clone)]
pub struct BrewSessionReference {
    id: String,
    batch_code: String,
    name: String,
}

#[repr(C)]
#[derive(Serialize, Deserialize, Debug, Clone)]
pub struct BrewSession {
    pub id: String,
    pub phase: String,
    pub batch_code: String,
    pub recipe_title: String,
    pub recipe_id: String,
    pub style_name: String,
    pub created_at: String,
}

impl BrewSessionReference {
    pub fn from_bf_session(session: &BfBrewSession) -> BrewSessionReference {
        BrewSessionReference {
            id: session.id.clone(),
            batch_code: session.batchcode.clone(),
            name: session.recipe_title.clone(),
        }
    }
}

impl BrewSession {
    pub fn from_bf_session(session: &BfBrewSessionFull) -> BrewSession {
        BrewSession {
            id: session.id.clone(),
            phase: session.phase.clone(),
            batch_code: session.batchcode.clone(),
            recipe_title: session.recipe_title.clone(),
            recipe_id: session.recipeid.clone(),
            style_name: session.recipe.stylename.clone(),
            created_at: session.created_at.clone(),
        }
    }
}

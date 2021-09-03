use serde::{Deserialize, Serialize};

use crate::data::brewers_friend::recipes::BeerXml;

#[derive(Serialize, Deserialize, Debug, Clone)]
pub struct Fermentable {
    pub name: String,
    pub amount: String,
}

#[derive(Serialize, Deserialize, Debug, Clone)]
pub struct Hop {
    pub name: String,
    pub amount: u16,
    pub time: String,
    pub hop_use: String,
    pub user_hop_use: Option<String>,
    pub hop_temp: Option<String>,
}

#[derive(Serialize, Deserialize, Debug, Clone)]
pub struct Yeast {
    pub name: String,
    pub amount: String,
}

#[derive(Serialize, Deserialize, Debug, Clone)]
pub struct MashStep {
    pub name: String,
    pub mash_step_type: String,
    pub step_time: Option<String>,
    pub infuse_amount: Option<String>,
    pub step_temp: Option<String>,
}

#[derive(Serialize, Deserialize, Debug, Clone)]
pub struct Mash {
    pub grain_temp: String,
    pub mash_steps: Vec<MashStep>,
}

#[derive(Serialize, Deserialize, Debug, Clone)]
pub struct Recipe {
    pub name: String,
    pub version: String,
    pub recipe_type: String,
    pub fermentables: Vec<Fermentable>,
    pub hops: Vec<Hop>,
    pub yeast: Vec<Yeast>,
    pub mash: Mash,
    pub boil_time: String,
    pub primary_temp: Option<String>,
}

impl Recipe {
    pub fn from_beer_xml(beer_xml: &BeerXml) -> Recipe {
        let recipe = &beer_xml.RECIPE;
        let fermentables = recipe.FERMENTABLES.FERMENTABLE.iter().map(|r| Fermentable {
            name: r.NAME.clone(),
            amount: r.AMOUNT.clone(),
        });
        let hops = recipe.HOPS.HOP.iter().map(|r| Hop {
            name: r.NAME.clone(),
            amount: (r.AMOUNT.clone().parse::<f64>().unwrap_or(0f64) * 1000f64) as u16, // Brewers friend uses KG, we use G
            time: r.TIME.clone(),
            hop_use: r.USE.clone(),
            user_hop_use: r.USER_HOP_USE.clone(),
            hop_temp: r.HOP_TEMP.clone(),
        });
        let yeast = recipe.YEASTS.YEAST.iter().map(|r| Yeast {
            name: r.NAME.clone(),
            amount: r.AMOUNT.clone(),
        });
        let mash_steps = recipe.MASH.MASH_STEPS.MASH_STEP.iter().map(|r| MashStep {
            name: r.NAME.clone(),
            mash_step_type: r.TYPE.clone(),
            step_time: r.STEP_TIME.clone(),
            infuse_amount: r.INFUSE_AMOUNT.clone(),
            step_temp: r.STEP_TEMP.clone(),
        });
        Recipe {
            name: recipe.NAME.clone(),
            version: recipe.VERSION.clone(),
            recipe_type: recipe.TYPE.clone(),
            fermentables: fermentables.collect(),
            hops: hops.collect(),
            yeast: yeast.collect(),
            mash: Mash {
                grain_temp: recipe.MASH.GRAIN_TEMP.clone(),
                mash_steps: mash_steps.collect(),
            },
            boil_time: recipe.BOIL_TIME.clone(),
            primary_temp: recipe.PRIMARY_TEMP.clone(),
        }
    }
}

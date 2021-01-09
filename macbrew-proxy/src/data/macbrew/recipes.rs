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
    pub amount: String,
    pub time: String,
}

#[derive(Serialize, Deserialize, Debug, Clone)]
pub struct Yeast {
    pub name: String,
    pub amount: String,
}

#[derive(Serialize, Deserialize, Debug, Clone)]
pub struct Recipe {
    pub name: String,
    pub version: String,
    pub recipe_type: String,
    pub fermentables: Vec<Fermentable>,
    pub hops: Vec<Hop>,
    pub yeast: Vec<Yeast>,
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
            amount: r.AMOUNT.clone(),
            time: r.TIME.clone(),
        });
        let yeast = recipe.YEASTS.YEAST.iter().map(|r| Yeast {
            name: r.NAME.clone(),
            amount: r.AMOUNT.clone(),
        });
        Recipe {
            name: recipe.NAME.clone(),
            version: recipe.VERSION.clone(),
            recipe_type: recipe.TYPE.clone(),
            fermentables: fermentables.collect(),
            hops: hops.collect(),
            yeast: yeast.collect(),
        }
    }
}

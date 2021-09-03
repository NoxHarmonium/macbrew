// See http://www.beerxml.com/

use serde::{Deserialize, Serialize};

#[derive(Serialize, Deserialize, Debug, Clone)]
#[allow(non_snake_case)]
pub struct BeerXmlFermentable {
    pub NAME: String,
    pub VERSION: String,
    pub TYPE: String,
    pub AMOUNT: String,
    pub YIELD: String,
    pub COLOR: String,
    pub ADD_AFTER_BOIL: Option<String>,
    pub ORIGIN: Option<String>,
    pub SUPPLIER: Option<String>,
    pub NOTES: Option<String>,
    pub COARSE_FINE_DIFF: Option<String>,
    pub MOISTURE: Option<String>,
    pub DIASTATIC_POWER: Option<String>,
    pub PROTEIN: Option<String>,
    pub MAX_IN_BATCH: Option<String>,
    pub RECOMMEND_MASH: Option<String>,
    pub IBU_GAL_PER_LB: Option<String>,
}

#[derive(Serialize, Deserialize, Debug, Clone)]
#[allow(non_snake_case)]
pub struct BeerXmlFermentables {
    pub FERMENTABLE: Vec<BeerXmlFermentable>,
}

#[derive(Serialize, Deserialize, Debug, Clone)]
#[allow(non_snake_case)]
pub struct BeerXmlHop {
    pub NAME: String,
    pub VERSION: String,
    pub ALPHA: String,
    pub AMOUNT: String,
    pub USE: String,
    pub TIME: String,
    pub NOTES: Option<String>,
    pub TYPE: Option<String>,
    pub FORM: Option<String>,
    pub BETA: Option<String>,
    pub HSI: Option<String>,
    pub ORIGIN: Option<String>,
    pub SUBSTITUTES: Option<String>,
    pub HUMULENE: Option<String>,
    pub CARYOPHYLLENE: Option<String>,
    pub COHUMULONE: Option<String>,
    pub MYRCENE: Option<String>,
    // Non standard
    pub USER_HOP_USE: Option<String>,
    pub HOP_TEMP: Option<String>,
    pub UTILIZATION: Option<String>,
    pub TEMPERATURE: Option<String>,
}

#[derive(Serialize, Deserialize, Debug, Clone)]
#[allow(non_snake_case)]
pub struct BeerXmlHops {
    pub HOP: Vec<BeerXmlHop>,
}

#[derive(Serialize, Deserialize, Debug, Clone)]
#[allow(non_snake_case)]
pub struct BeerXmlYeast {
    pub NAME: String,
    pub VERSION: String,
    pub TYPE: String,
    pub FORM: String,
    pub AMOUNT: String,
    pub AMOUNT_IS_WEIGHT: Option<bool>,
    pub LABORATORY: Option<String>,
    pub PRODUCT_ID: Option<String>,
    pub MIN_TEMPERATURE: Option<String>,
    pub MAX_TEMPERATURE: Option<String>,
    pub FLOCULATION: Option<String>,
    pub ATTENUATION: Option<String>,
    pub NOTES: Option<String>,
    pub BEST_FOR: Option<String>,
    pub TIMES_CULTURED: Option<String>,
    pub MAX_REUSE: Option<String>,
    pub ADD_TO_SECONDARY: Option<String>,
}

#[derive(Serialize, Deserialize, Debug, Clone)]
#[allow(non_snake_case)]
pub struct BeerXmlYeasts {
    pub YEAST: Vec<BeerXmlYeast>,
}

#[derive(Serialize, Deserialize, Debug, Clone)]
#[allow(non_snake_case)]
pub struct BeerXmlMashStep {
    pub NAME: String,
    pub TYPE: String,
    pub STEP_TIME: Option<String>,
    pub INFUSE_AMOUNT: Option<String>,
    pub STEP_TEMP: Option<String>,
}

#[derive(Serialize, Deserialize, Debug, Clone)]
#[allow(non_snake_case)]
pub struct BeerXmlMashSteps {
    pub MASH_STEP: Vec<BeerXmlMashStep>,
}

#[derive(Serialize, Deserialize, Debug, Clone)]
#[allow(non_snake_case)]
pub struct BeerXmlMash {
    pub GRAIN_TEMP: String,
    pub MASH_STEPS: BeerXmlMashSteps,
}

#[derive(Serialize, Deserialize, Debug, Clone)]
#[allow(non_snake_case)]
pub struct BeerXmlRecipe {
    pub NAME: String,
    pub VERSION: String,
    pub TYPE: String,
    // pub STYLE: TODO
    // pub EQUIPMENT: TODO
    pub BREWER: String,
    pub ASST_BREWER: Option<String>,
    pub BATCH_SIZE: String,
    pub BOIL_SIZE: String,
    pub BOIL_TIME: String,
    pub EFFICIENCY: Option<String>,
    pub HOPS: BeerXmlHops,
    pub FERMENTABLES: BeerXmlFermentables,
    //pub MISCS: TODO
    pub YEASTS: BeerXmlYeasts,
    //pub WATERS: TODO
    pub MASH: BeerXmlMash,
    pub NOTES: Option<String>,
    pub TASTE_NOTES: Option<String>,
    pub TASTE_RATING: Option<String>,
    pub OG: Option<String>,
    pub FG: Option<String>,
    pub FERMENTATION_STAGES: Option<i32>,
    pub PRIMARY_AGE: Option<String>,
    pub PRIMARY_TEMP: Option<String>,
    pub SECONDARY_AGE: Option<String>,
    pub SECONDARY_TEMP: Option<String>,
    pub TERTIARY_AGE: Option<String>,
    pub TERTIARY_TEMP: Option<String>,
    pub AGE: Option<String>,
    pub AGE_TEMP: Option<String>,
    pub DATE: Option<String>,
    pub CARBONATION: Option<String>,
    pub FORCED_CARBONATION: Option<bool>,
    pub PRIMING_SUGAR_NAME: Option<String>,
    pub CARBONATION_TEMP: Option<String>,
    pub PRIMING_SUGAR_EQUIV: Option<String>,
    pub KEG_PRIMING_FACTOR: Option<String>,

    // Non standard
    pub EST_COLOR: Option<String>,
    pub IBU: Option<String>,
    pub IBU_METHOD: Option<String>,
    pub EST_ABV: Option<String>,
    pub EST_OG: Option<String>,
    pub EST_FG: Option<String>,
}

#[derive(Serialize, Deserialize, Debug, Clone)]
#[allow(non_snake_case)]
pub struct BeerXml {
    pub RECIPE: BeerXmlRecipe,
}

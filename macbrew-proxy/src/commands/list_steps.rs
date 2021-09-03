use crate::commands::command::{prepare_response, Command};
use crate::data::brewers_friend::bf_data_manager::BfDataManager;
use crate::data::macbrew::recipes::Recipe;
use crate::data::macbrew::steps::{
    BrewSessionStep, BOIL_PHASE, CHILL_PHASE, CLEANUP_PHASE, MASH_PHASE, PREPARE_PHASE,
};
use crate::error::Error::InvalidCommandInput;
use crate::error::Result;
use async_trait::async_trait;
use std::marker::PhantomData;
use std::str::FromStr;

// TODO: Work out why rustc thinks T is unused. Is PhantomData needed? Can I restructure this?
// Clue: https://github.com/rust-lang/rust/issues/23246
pub struct ListStepsCommand<DataManager: BfDataManager>(PhantomData<DataManager>);

#[async_trait]
impl<DataManager: BfDataManager + 'static> Command<DataManager> for ListStepsCommand<DataManager> {
    async fn handle(rid: &str, args: &[&str]) -> Result<Vec<u8>> {
        // TODO: This is extremely rough and just a MVP implmentation to start working on the Mac UI
        //       It will be cleaned up soon
        match args {
            [session_id] => {
                let session = DataManager::session(session_id).await?;
                let beer_xml_recipe = DataManager::recipe(&session.recipeid).await?;
                let recipe = Recipe::from_beer_xml(&beer_xml_recipe);

                let initial_steps = vec![
                    BrewSessionStep {
                        description: "Check ingredients (hops/yeast etc.) and reset equipment"
                            .to_string(),
                        time: -1,
                        phase: PREPARE_PHASE,
                    },
                    BrewSessionStep {
                        description: "Prepare strike water (add salt/acid)".to_string(),
                        time: -1,
                        phase: PREPARE_PHASE,
                    },
                ];

                let mash_hops = recipe
                    .hops
                    .iter()
                    .filter(|h| h.hop_use == "Mash")
                    .collect::<Vec<_>>();

                let mash_hop_steps: Vec<BrewSessionStep> = if mash_hops.is_empty() {
                    vec![]
                } else {
                    mash_hops
                        .iter()
                        .map(|s| {
                            // TODO: Clean up this mess
                            let coerced_time = i16::from_str(&s.time).unwrap_or(0);
                            BrewSessionStep {
                                description: format!(
                                    "Add {:?}g of {:?} hops to mash at minute {:?}",
                                    s.amount, s.name, s.time,
                                ),
                                time: coerced_time,
                                phase: MASH_PHASE,
                            }
                        })
                        .collect()
                };

                let mash_steps = recipe
                    .mash
                    .mash_steps
                    .iter()
                    .map(|s| {
                        // TODO: Clean up this mess
                        let time: String = s.step_time.clone().unwrap_or_else(|| "".to_string());
                        let coerced_time = i16::from_str(&time).unwrap_or(0);
                        BrewSessionStep {
                            description: format!(
                                "Heat {:?} water to {:?} degrees",
                                s.mash_step_type,
                                s.step_temp.clone().unwrap_or_else(|| "unknown".to_string())
                            ),
                            time: coerced_time,
                            phase: MASH_PHASE,
                        }
                    })
                    .collect::<Vec<BrewSessionStep>>();

                let preboil_steps = vec![
                    BrewSessionStep {
                        description: "Take gravity sample and record pre-boil volume".to_string(),
                        time: -1,
                        phase: MASH_PHASE,
                    },
                    BrewSessionStep {
                        description: "Bring to a boil".to_string(),
                        time: -1,
                        phase: MASH_PHASE,
                    },
                ];

                let boil_hops = recipe
                    .hops
                    .iter()
                    .filter(|h| h.hop_use == "Boil")
                    .collect::<Vec<_>>();

                let boil_hop_steps: Vec<BrewSessionStep> = if boil_hops.is_empty() {
                    vec![]
                } else {
                    boil_hops
                        .iter()
                        .map(|s| {
                            // TODO: Clean up this mess
                            let coerced_time = i16::from_str(&s.time).unwrap_or(0);
                            BrewSessionStep {
                                description: format!(
                                    "Add {:?}g of {:?} hops to boil at minute {:?}",
                                    s.amount, s.name, s.time,
                                ),
                                time: coerced_time,
                                phase: BOIL_PHASE,
                            }
                        })
                        .collect()
                };

                let postboil_steps = vec![
                    BrewSessionStep {
                        description: "Turn off kettle".to_string(),
                        time: i16::from_str(&recipe.boil_time).unwrap_or(0),
                        phase: BOIL_PHASE,
                    },
                    BrewSessionStep {
                        description: "Start chilling wort".to_string(),
                        time: -1,
                        phase: BOIL_PHASE,
                    },
                ];

                let whirlpool_hops = recipe
                    .hops
                    .iter()
                    .filter(|h| {
                        // TODO: Maybe move some of this coersion to where the XML recipe is convered to rust objects
                        h.hop_use == "Aroma"
                            && h.user_hop_use.clone().unwrap_or_else(|| "".to_string())
                                == "Whirlpool"
                    })
                    .collect::<Vec<_>>();

                let whirlpool_hop_steps: Vec<BrewSessionStep> = if whirlpool_hops.is_empty() {
                    vec![]
                } else {
                    whirlpool_hops
                        .iter()
                        .map(|s|
                            BrewSessionStep {
                                description: format!(
                                    "When temp is {:?} degrees, add {:?}g of {:?} hops to whirlpool for {:?} minutes",
                                    s.hop_temp, s.amount, s.name, s.time
                                ),
                                time: -1,
                                phase: CHILL_PHASE,
                            }
                        )
                        .collect()
                };

                let chill_steps = vec![
                    BrewSessionStep {
                        description: format!("Cool wort down to {:?}", recipe.primary_temp),
                        time: -1,
                        phase: CHILL_PHASE,
                    },
                    BrewSessionStep {
                        description: "Take gravity reading".to_string(),
                        time: -1,
                        phase: CHILL_PHASE,
                    },
                    BrewSessionStep {
                        description: "Transfer to fermentor".to_string(),
                        time: -1,
                        phase: CHILL_PHASE,
                    },
                    BrewSessionStep {
                        description: "Pitch yeast".to_string(),
                        time: -1,
                        phase: CHILL_PHASE,
                    },
                ];

                let cleanup_steps = vec![
                    BrewSessionStep {
                        description: "Set fermentation chamber temp".to_string(),
                        time: -1,
                        phase: CLEANUP_PHASE,
                    },
                    BrewSessionStep {
                        description: "Cleanup equipment".to_string(),
                        time: -1,
                        phase: CLEANUP_PHASE,
                    },
                ];

                let all_steps: Vec<BrewSessionStep> = [
                    initial_steps,
                    mash_hop_steps,
                    mash_steps,
                    preboil_steps,
                    boil_hop_steps,
                    postboil_steps,
                    whirlpool_hop_steps,
                    chill_steps,
                    cleanup_steps,
                ]
                .concat();
                prepare_response(rid, true, &all_steps)
            }
            [_args @ ..] => Err(InvalidCommandInput {
                message: String::from("Expected single parameter (Session ID)"),
            }),
        }
    }
}

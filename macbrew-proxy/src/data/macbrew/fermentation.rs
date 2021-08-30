use crate::data::brewers_friend::fermentation::{BfFermentationReading, BfFermentationResponse};
use crate::data::macbrew::shared::{iso_string_to_mac_epoch, MacEpoch};
use serde::{Deserialize, Serialize};
use std::str::FromStr;

type GraphPoint = (i8, u8);

/**
 * Ensures that the temp reading is in Celsius because I can't read Fahrenheit
 * TODO: Make this switchable somewhere for US users
 */
fn extract_temp_from_reading(reading: &BfFermentationReading) -> f64 {
    // TODO: Handle parse error
    let raw_temp = f64::from_str(&reading.temp).unwrap();
    match reading.temp_unit.as_str() {
        "F" => (raw_temp - 32f64) * 0.5556f64,
        "C" => raw_temp,
        // TODO: Handle invalid value
        &_ => raw_temp,
    }
}

#[repr(C)]
#[derive(Serialize, Deserialize, Debug, Clone)]
pub struct FermentationData {
    pub graph: Vec<GraphPoint>,
    pub start_date: MacEpoch,
    pub end_date: MacEpoch,
}

impl FermentationData {
    pub fn from_bf_fermentation(
        fermentation_response: &BfFermentationResponse,
    ) -> FermentationData {
        // Width of the graph control in pixels
        let width = 320;
        // How many pixels per point (saves bandwidth)
        let divider = 4;
        // How many buckets (data points) needed for the graph
        let bucket_count = width / divider;
        // Filter out any readings that are missing temp or gravity
        // TODO: Manual entries can have one or the other, maybe we need to
        // do something like fill in missing values with previous values.
        // This is fine for my current use case though (getting readings from a Tilt)
        let filtered_readings: Vec<_> = fermentation_response
            .readings
            .iter()
            .filter(|reading| !reading.temp.is_empty() && !reading.gravity.is_empty())
            .collect();
        let reading_count = filtered_readings.len();

        // Allocate each data point
        let mut buckets = vec![(0, 0); bucket_count];

        // Fill in the pre allocated vectore with points
        // TODO: This is a bad way to do it and assumes that the data points are spread evenly
        //       what we really need to do is use time as the index and do a linear interpolation
        //       to fill in missing points. It will do to test the Mac UI control
        for (bucket_index, point) in buckets.iter_mut().enumerate() {
            // Group each reading into a "bucket" that covers the size of a data point
            let readings_per_bucket = reading_count / bucket_count;
            let reading_index = bucket_index * readings_per_bucket;
            let readings_to_average =
                &filtered_readings[reading_index..reading_index + readings_per_bucket];

            // Average the readings over an entire bucket
            let average_temp_for_bucket: f64 = readings_to_average
                .iter()
                .map(|reading| extract_temp_from_reading(reading))
                .sum::<f64>()
                / (readings_per_bucket as f64);

            let average_gravity_for_bucket: f64 = readings_to_average
                .iter()
                // TODO: Handle parse error
                .map(|reading| f64::from_str(&reading.gravity).unwrap())
                .sum::<f64>()
                / (readings_per_bucket as f64);

            // Storing the literal SG reading as fixed point or string is wasteful
            // If we ignore that gravity can dip slightly below 1.000 sometimes and assume
            // that gravity will never be higher that 1.256 (a safe assumption), we can drop
            // the 1 and convert it for display purposes on the Mac side
            // TODO: Extract to functions
            let fixed_point_gravity = ((average_gravity_for_bucket * 1000f64).round() as i32
                - 1000)
                .clamp(0, u8::MAX.into());

            *point = (average_temp_for_bucket as i8, fixed_point_gravity as u8);
        }

        FermentationData {
            graph: buckets,
            // TODO: Handle 0 size readings error properly
            start_date: iso_string_to_mac_epoch(
                &fermentation_response.readings.first().unwrap().created_at,
            ),
            end_date: iso_string_to_mac_epoch(
                &fermentation_response.readings.last().unwrap().created_at,
            ),
        }
    }
}

use chrono::DateTime;

/**
 * This date-time information is expressed, using 4 bytes, as the number of seconds elapsed since midnight, January 1, 1904.
 */
pub type MacEpoch = u32;

pub fn iso_string_to_mac_epoch(iso_string: &str) -> MacEpoch {
    // TODO: Handle parse error
    let parsed_date = DateTime::parse_from_rfc3339(iso_string).unwrap();
    let unix_epoch = DateTime::parse_from_rfc3339("1970-01-01T00:00:00Z").unwrap();
    let mac_epoch = DateTime::parse_from_rfc3339("1904-01-01T00:00:00Z").unwrap();
    let epoch_difference = mac_epoch.signed_duration_since(unix_epoch);
    // Returns the number of non-leap seconds since January 1, 1970 0:00:00 UTC (aka "UNIX timestamp").
    (parsed_date.timestamp() + epoch_difference.num_seconds()) as MacEpoch
}

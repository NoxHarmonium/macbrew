use std::convert::TryInto;

pub fn xor_checksum_from_bytes(bytes: &[u8]) -> u32 {
    // I'll try a 32 bit checksum and see how it goes
    // The m68k processor can work on 32 bits (longs) so it will probably be fine
    // Can be reduced to 16 bit if needed
    let word_size = 4; // bytes
    let payload_size = bytes.len();
    let padding = 4 - (payload_size % word_size);

    // TODO: Does clone copy the payload? Is this a cheap operation?
    let mut padded_payload = bytes.to_vec();
    // If the payload does not break into word size cleanly, pad the remainder with zeros
    padded_payload.resize(payload_size + padding, 0);

    // Do a basic XOR checksum
    // https://en.wikipedia.org/wiki/Checksum#Parity_byte_or_parity_word
    padded_payload
        .chunks_exact(word_size)
        .map(|word| {
            u32::from_le_bytes(
                word.try_into()
                    .expect("chunks_exact should always pass the correct length"),
            )
        })
        .fold(0, |a, b| a ^ b)
}

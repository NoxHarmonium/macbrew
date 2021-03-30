use serde::{ser, Serialize};

use crate::error::{Error, Result};

use encoding::all::MAC_ROMAN;
use encoding::{EncoderTrap, Encoding};

// Hacked out of the postcard library
pub fn serialize<T>(value: &T) -> Result<Vec<u8>>
where
    T: Serialize + ?Sized,
{
    let mut serializer = Serializer {
        output: StdVec(Vec::new()),
    };
    value.serialize(&mut serializer)?;
    serializer
        .output
        .release()
        .map_err(|_| Error::SerializeBufferFull {})
}

/// The `StdVec` flavor is a wrapper type around a `std::vec::Vec`.
pub struct StdVec(pub Vec<u8>);

#[allow(clippy::unnecessary_wraps)]
impl StdVec {
    #[inline(always)]
    fn try_extend(&mut self, data: &[u8]) -> core::result::Result<(), ()> {
        self.0.extend_from_slice(data);
        Ok(())
    }

    #[inline(always)]
    fn try_push(&mut self, data: u8) -> core::result::Result<(), ()> {
        self.0.push(data);
        Ok(())
    }

    fn release(self) -> core::result::Result<Vec<u8>, ()> {
        Ok(self.0)
    }
}

// impl Index<usize> for StdVec {
//     type Output = u8;

//     fn index(&self, idx: usize) -> &u8 {
//         &self.0[idx]
//     }
// }

// impl IndexMut<usize> for StdVec {
//     fn index_mut(&mut self, idx: usize) -> &mut u8 {
//         &mut self.0[idx]
//     }
// }

// TODO: Clean this up
// TODO: Casting from u32 to u16 without a check is probably dangerous. Better check what happens

/// Based on Postcard serializer but simpler and more classic Mac compatible
/// See: https://github.com/jamesmunns/postcard/blob/master/src/ser/serializer.rs
pub struct Serializer {
    /// This is the Flavor(s) that will be used to modify or store any bytes generated
    /// by serialization
    pub output: StdVec,
}

impl<'a> ser::Serializer for &'a mut Serializer {
    type Ok = ();

    type Error = Error;

    // Associated types for keeping track of additional state while serializing
    // compound data structures like sequences and maps. In this case no
    // additional state is required beyond what is already stored in the
    // Serializer struct.
    type SerializeSeq = Self;
    type SerializeTuple = Self;
    type SerializeTupleStruct = Self;
    type SerializeTupleVariant = Self;
    type SerializeMap = Self;
    type SerializeStruct = Self;
    type SerializeStructVariant = Self;

    fn is_human_readable(&self) -> bool {
        false
    }

    fn serialize_bool(self, v: bool) -> Result<()> {
        self.serialize_u8(if v { 1 } else { 0 })
    }

    fn serialize_i8(self, v: i8) -> Result<()> {
        self.serialize_u8(v.to_be_bytes()[0])
    }

    fn serialize_i16(self, v: i16) -> Result<()> {
        self.output
            .try_extend(&v.to_be_bytes())
            .map_err(|_| Error::SerializeBufferFull {})
    }

    fn serialize_i32(self, v: i32) -> Result<()> {
        self.output
            .try_extend(&v.to_be_bytes())
            .map_err(|_| Error::SerializeBufferFull {})
    }

    fn serialize_i64(self, v: i64) -> Result<()> {
        self.output
            .try_extend(&v.to_be_bytes())
            .map_err(|_| Error::SerializeBufferFull {})
    }

    fn serialize_u8(self, v: u8) -> Result<()> {
        self.output
            .try_push(v)
            .map_err(|_| Error::SerializeBufferFull {})
    }

    fn serialize_u16(self, v: u16) -> Result<()> {
        self.output
            .try_extend(&v.to_be_bytes())
            .map_err(|_| Error::SerializeBufferFull {})
    }

    fn serialize_u32(self, v: u32) -> Result<()> {
        self.output
            .try_extend(&v.to_be_bytes())
            .map_err(|_| Error::SerializeBufferFull {})
    }

    fn serialize_u64(self, v: u64) -> Result<()> {
        self.output
            .try_extend(&v.to_be_bytes())
            .map_err(|_| Error::SerializeBufferFull {})
    }

    fn serialize_f32(self, v: f32) -> Result<()> {
        let buf = v.to_bits().to_be_bytes();
        self.output
            .try_extend(&buf)
            .map_err(|_| Error::SerializeBufferFull {})
    }

    fn serialize_f64(self, v: f64) -> Result<()> {
        let buf = v.to_bits().to_be_bytes();
        self.output
            .try_extend(&buf)
            .map_err(|_| Error::SerializeBufferFull {})
    }

    fn serialize_char(self, v: char) -> Result<()> {
        let mut buf = [0u8; 4];
        let char_as_string = v.encode_utf8(&mut buf);
        let mac_encoded = MAC_ROMAN
            .encode(char_as_string, EncoderTrap::Replace)
            .unwrap();
        mac_encoded.serialize(self)
    }

    fn serialize_str(self, v: &str) -> Result<()> {
        self.output
            .try_extend(&(v.len() as u16).to_be_bytes())
            .map_err(|_| Error::SerializeBufferFull {})?;
        self.output
            .try_extend(v.as_bytes())
            .map_err(|_| Error::SerializeBufferFull {})?;
        Ok(())
    }

    fn serialize_bytes(self, v: &[u8]) -> Result<()> {
        self.output
            .try_extend(&(v.len() as u16).to_be_bytes())
            .map_err(|_| Error::SerializeBufferFull {})?;
        self.output
            .try_extend(v)
            .map_err(|_| Error::SerializeBufferFull {})
    }

    fn serialize_none(self) -> Result<()> {
        self.serialize_u8(0)
    }

    fn serialize_some<T>(self, value: &T) -> Result<()>
    where
        T: ?Sized + Serialize,
    {
        self.serialize_u8(1)?;
        value.serialize(self)
    }

    fn serialize_unit(self) -> Result<()> {
        Ok(())
    }

    fn serialize_unit_struct(self, _name: &'static str) -> Result<()> {
        Ok(())
    }

    fn serialize_unit_variant(
        self,
        _name: &'static str,
        variant_index: u32,
        _variant: &'static str,
    ) -> Result<()> {
        self.output
            .try_extend(&(variant_index as u16).to_be_bytes())
            .map_err(|_| Error::SerializeBufferFull {})
    }

    fn serialize_newtype_struct<T>(self, _name: &'static str, value: &T) -> Result<()>
    where
        T: ?Sized + Serialize,
    {
        value.serialize(self)
    }

    fn serialize_newtype_variant<T>(
        self,
        _name: &'static str,
        variant_index: u32,
        _variant: &'static str,
        value: &T,
    ) -> Result<()>
    where
        T: ?Sized + Serialize,
    {
        self.output
            .try_extend(&(variant_index as u16).to_be_bytes())
            .map_err(|_| Error::SerializeBufferFull {})?;
        value.serialize(self)
    }

    fn serialize_seq(self, len: Option<usize>) -> Result<Self::SerializeSeq> {
        self.output
            .try_extend(&(len.ok_or(Error::SerializeSeqLengthUnknown {})? as u16).to_be_bytes())
            .map_err(|_| Error::SerializeBufferFull {})?;
        Ok(self)
    }

    fn serialize_tuple(self, _len: usize) -> Result<Self::SerializeTuple> {
        Ok(self)
    }

    fn serialize_tuple_struct(
        self,
        _name: &'static str,
        _len: usize,
    ) -> Result<Self::SerializeTupleStruct> {
        Ok(self)
    }

    fn serialize_tuple_variant(
        self,
        _name: &'static str,
        variant_index: u32,
        _variant: &'static str,
        _len: usize,
    ) -> Result<Self::SerializeTupleVariant> {
        self.output
            .try_extend(&(variant_index as u16).to_be_bytes())
            .map_err(|_| Error::SerializeBufferFull {})?;
        Ok(self)
    }

    fn serialize_map(self, len: Option<usize>) -> Result<Self::SerializeMap> {
        self.output
            .try_extend(&(len.ok_or(Error::SerializeSeqLengthUnknown {})? as u16).to_be_bytes())
            .map_err(|_| Error::SerializeBufferFull {})?;
        Ok(self)
    }

    fn serialize_struct(self, _name: &'static str, _len: usize) -> Result<Self::SerializeStruct> {
        Ok(self)
    }

    fn serialize_struct_variant(
        self,
        _name: &'static str,
        variant_index: u32,
        _variant: &'static str,
        _len: usize,
    ) -> Result<Self::SerializeStructVariant> {
        self.output
            .try_extend(&(variant_index as u16).to_be_bytes())
            .map_err(|_| Error::SerializeBufferFull {})?;
        Ok(self)
    }

    fn collect_str<T: ?Sized>(self, _value: &T) -> Result<Self::Ok>
    where
        T: core::fmt::Display,
    {
        unreachable!()
    }
}

impl<'a> ser::SerializeSeq for &'a mut Serializer {
    // Must match the `Ok` type of the serializer.
    type Ok = ();
    // Must match the `Error` type of the serializer.
    type Error = Error;

    // Serialize a single element of the sequence.
    fn serialize_element<T>(&mut self, value: &T) -> Result<()>
    where
        T: ?Sized + Serialize,
    {
        value.serialize(&mut **self)
    }

    // Close the sequence.
    fn end(self) -> Result<()> {
        Ok(())
    }
}

impl<'a> ser::SerializeTuple for &'a mut Serializer {
    type Ok = ();
    type Error = Error;

    fn serialize_element<T>(&mut self, value: &T) -> Result<()>
    where
        T: ?Sized + Serialize,
    {
        value.serialize(&mut **self)
    }

    fn end(self) -> Result<()> {
        Ok(())
    }
}

impl<'a> ser::SerializeTupleStruct for &'a mut Serializer {
    type Ok = ();
    type Error = Error;

    fn serialize_field<T>(&mut self, value: &T) -> Result<()>
    where
        T: ?Sized + Serialize,
    {
        value.serialize(&mut **self)
    }

    fn end(self) -> Result<()> {
        Ok(())
    }
}

impl<'a> ser::SerializeTupleVariant for &'a mut Serializer {
    type Ok = ();
    type Error = Error;

    fn serialize_field<T>(&mut self, value: &T) -> Result<()>
    where
        T: ?Sized + Serialize,
    {
        value.serialize(&mut **self)
    }

    fn end(self) -> Result<()> {
        Ok(())
    }
}

impl<'a> ser::SerializeMap for &'a mut Serializer {
    type Ok = ();
    type Error = Error;

    fn serialize_key<T>(&mut self, key: &T) -> Result<()>
    where
        T: ?Sized + Serialize,
    {
        key.serialize(&mut **self)
    }

    fn serialize_value<T>(&mut self, value: &T) -> Result<()>
    where
        T: ?Sized + Serialize,
    {
        value.serialize(&mut **self)
    }

    fn end(self) -> Result<()> {
        Ok(())
    }
}

impl<'a> ser::SerializeStruct for &'a mut Serializer {
    type Ok = ();
    type Error = Error;

    fn serialize_field<T>(&mut self, _key: &'static str, value: &T) -> Result<()>
    where
        T: ?Sized + Serialize,
    {
        value.serialize(&mut **self)
    }

    fn end(self) -> Result<()> {
        Ok(())
    }
}

impl<'a> ser::SerializeStructVariant for &'a mut Serializer {
    type Ok = ();
    type Error = Error;

    fn serialize_field<T>(&mut self, _key: &'static str, value: &T) -> Result<()>
    where
        T: ?Sized + Serialize,
    {
        value.serialize(&mut **self)
    }

    fn end(self) -> Result<()> {
        Ok(())
    }
}

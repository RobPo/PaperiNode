/*
// This is the code you will need to add to the 'decoder' section in the TTN backend to let the
// weather forecast demo run properly.

function Decoder(bytes, port) {
  var decoded = {
    counter: bytes[2],
    battery: (bytes[5] << 8 | bytes[6])/100
  };
  return decoded;
}
*/

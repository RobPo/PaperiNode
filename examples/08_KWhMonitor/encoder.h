function Encoder(object, port) {
  // Encode downlink messages sent as
  // object to an array or buffer of bytes.
  var bytes = [12];

  bytes[0] = parseInt(object.date);
  bytes[2] = parseInt(object.month);
  bytes[4] = parseInt(object.hour);
  bytes[6] = parseInt(object.mins);
  bytes[8] = parseInt(object.kWh);
 
  return bytes;
}
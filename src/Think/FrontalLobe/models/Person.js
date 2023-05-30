const mongoose = require('mongoose');
const Schema = mongoose.Schema;

// Create Schema
const PersonSchema = new Schema({
  name: {
    type: String,
    required: true
  },
  age: {
    type: String,
    required: true
  },
  description: {
    type: String,
    required: true
  },
  status: {
    type: String,
    default: 'active'
  }
});


module.exports = Person = mongoose.model('Person', PersonSchema);

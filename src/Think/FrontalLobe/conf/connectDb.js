const mongoose = require('mongoose');

// O endereço do MongoDB é o nome do serviço no Docker Compose (neste caso, "mongodb")
const dbAddress = 'mongodb://sentinel_memory:27017/sentinel-eyes';

mongoose.connect(dbAddress, { useNewUrlParser: true, useUnifiedTopology: true })
  .then(() => console.log('MongoDB Connected...'))
  .catch(err => console.log(err));

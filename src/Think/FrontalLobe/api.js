const express = require('express');
const multer = require('multer');
const path = require('path');
const router = express.Router();
const PersonController = require('./controllers/PersonController');

const storage = multer.diskStorage({
    destination: function(req, file, cb) {
      cb(null, '/app/faces_bd/');
    },
    filename: function(req, file, cb) {
      cb(null, file.originalname);
    }
  });
  
const upload = multer({ storage: storage });


router.post('/memory/add', upload.single('photo'), PersonController.addPerson);

router.get('/memory/get_all_people', PersonController.getAllPeople);
router.get('/memory/auth', PersonController.authPerson);

router.delete('/memory/forget', PersonController.forgetPerson);




module.exports = router;

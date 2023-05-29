const express = require('express');
const router = express.Router();

const PersonController = require("./controllers/PersonController");



router.post('/memory/add', PersonController.addPerson);

router.get('/memory/get_all_people', PersonController.getAllPeople);
router.get('/memory/auth', PersonController.authPerson);

router.delete('/memory/forget', PersonController.forgetPerson);




module.exports = router;

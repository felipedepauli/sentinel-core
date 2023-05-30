/**
 * @module controllers/PersonController
 */
const path = require('path');
const fs = require('fs');
const WebSocket = require('ws')
const Person = require('../models/Person');

/**
 * A controller for handling requests related to Person.
 * @class
 */
const PersonController = {
  /**
   * Adds a new person to the database.
   *
   * @async
   * @function
   * @param {object} req - The request object. The new person's data is expected to be in req.body.
   * @param {object} res - The response object. The newly created person will be sent in the response if successful.
   * @returns {object} - The created Person object if successful, or an error message if unsuccessful.
   */
  addPerson: async (req, res) => {
    let newPerson = new Person({
      name: req.body.name,
      age: req.body.age,
      description: req.body.description,
      status: req.body.status
    });

    try {
      // Salva a nova pessoa no banco de dados
      newPerson = await newPerson.save();
  
      // Agora, newPerson._id contém o ID gerado pelo MongoDB
  
      // Cria o novo nome do arquivo
      const newName = `${newPerson._id}__${newPerson.name.replace(/\s+/g, '_')}.jpg`;
  
      // Obtém o caminho atual do arquivo e o novo caminho
      const currentPath = req.file.path;
      const newPath = path.join('/app/faces_bd/', newName);
  
      // Renomeia o arquivo
      fs.rename(currentPath, newPath, function(err) {
        if (err) {
          res.status(500).json('Error: ' + err);
        }
      });
  
      // Atualiza o caminho da foto na entrada do banco de dados
      newPerson.photo = newPath;
      await newPerson.save();
  
      res.json(newPerson);
    } catch (err) {
      res.status(400).json('Error: ' + err);
    }
  },
  
  /**
   * Retrieves all people from the database.
   *
   * @async
   * @function
   * @param {object} req - The request object.
   * @param {object} res - The response object. An array of all people will be sent in the response if successful.
   * @returns {Array} - An array of Person objects if successful, or an error message if unsuccessful.
   */
  getAllPeople: async (req, res) => {},

  /**
   * Authenticates a person. 
   *
   * @async
   * @function
   * @param {object} req - The request object.
   * @param {object} res - The response object. A success message will be sent in the response if successful.
   * @returns {object} - A success message if successful, or an error message if unsuccessful.
   */
  authPerson: async (req, res) => {
    console.log("Time to authenticate!")
  
    // Cria um WebSocket para se conectar ao Python
    const ws = new WebSocket('ws://localhost:8081/auth');
  
    // Quando o WebSocket é aberto, envia o id da pessoa
    ws.on('open', function open() {
      ws.send(JSON.stringify({ id: req.body.id }));
    });
  
    // Quando uma mensagem é recebida do WebSocket
    ws.on('message', async function incoming(data) {
      try {
        // O id da pessoa é recebido do Python
        const personId = JSON.parse(data).id;
  
        const person = await Person.findOne({ _id: personId });
  
        if (!person) {
          return res.status(404).json({ message: "No person found with this ID" });
        }
  
        res.json(person);
      } catch (err) {
        res.status(500).json({ message: err.message });
      }
    });
  },
  

  /**
   * Deletes a person from the database.
   *
   * @async
   * @function
   * @param {object} req - The request object. The ID of the person to delete is expected to be in req.body.
   * @param {object} res - The response object. A success message will be sent in the response if successful.
   * @returns {object} - A success message if successful, or an error message if unsuccessful.
   */
  forgetPerson: async (req, res) => {},
}

module.exports = PersonController;
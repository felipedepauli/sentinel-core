/**
 * @module controllers/PersonController
 */
const path = require('path');
const fs = require('fs');
const WebSocket = require('ws')
const Person = require('../models/Person');
const util = require('util');
const exec = util.promisify(require('child_process').exec);

const FIFO_PATH = '/app/faces_bd/synapse';

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
    console.log("Time to authenticate!");
  
    try {
      // Verifica se o arquivo fifo existe
      if (!fs.existsSync(FIFO_PATH)) {
        // Se não existir, cria o arquivo fifo usando mkfifo
        await exec(`mkfifo ${FIFO_PATH}`);
      }
  
      // Ler o arquivo FIFO
      const fifoReadStream = fs.createReadStream(FIFO_PATH);
  
      let data = '';
      for await (const chunk of fifoReadStream) {
        data += chunk;
      }
  
      // Fecha o arquivo FIFO
      fifoReadStream.close();
  
      console.log(data)
  
      // Exclui o arquivo FIFO
      fs.unlinkSync(FIFO_PATH);
  
      if (data == "-1") {
        throw new Error("no person detected.")
      }
      // Solicitar ao banco de dados o objeto referente ao id
      const person = await Person.findById(data);
  
      if (!person) {
        console.error(`Person not found for id: ${data}`);
        return res.status(404).json({ error: 'Person not found' });
      }
  
      // Retorna para o frontend o objeto {"id": objeto_do_mongodb}
      res.json({ "id": person });
  
    } catch (error) {
      console.error(`Error during authentication: ${error}`);
      res.status(500).json({ error: 'Error during authentication' });
    }
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
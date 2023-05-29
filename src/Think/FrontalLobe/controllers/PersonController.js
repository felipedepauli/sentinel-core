/**
 * @module controllers/PersonController
 */

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
    const newPerson = new Person({
      name: req.body.name,
      email: req.body.email,
      password: req.body.password,
    });
  
    newPerson
      .save() // isso salva a nova pessoa no banco de dados
      .then(person => res.json(person)) // e depois envia de volta a nova pessoa como resposta
      .catch(err => res.status(400).json('Error: ' + err));
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
  authPerson: async (req, res) => {},

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
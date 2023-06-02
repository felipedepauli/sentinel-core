import axios from 'axios'; // Não esqueça de instalar isso via npm ou yarn
import { Button } from 'antd';
import React from 'react';
import iconMap from './IconMap.js';

const AuthButton = ({ setPersonInfo }) => {
  const authenticate = () => {
    setPersonInfo({
      name: 'Procurando o inimigo malvado!',
      age: "",
      status: "",
      description: 'Ai se eu encontro ele....'
    });

    axios.get('http://localhost:8081/memory/auth')
      .then((response) => {
        console.log(response.data); // Isto deve imprimir o objeto da pessoa no console do navegador
        setPersonInfo({
          age: response.data.id.age,
          name: response.data.id.name,
          description: response.data.id.description,
          status: response.data.id.status
        })
      })
      .catch((error) => {
        console.error(error);
      });
      console.log("Executei a parada aqui")
  }

  return (
      <Button size="large"  onClick={authenticate}>
         {iconMap["GiTargetDummy"]}
      </Button>
  )
};

export default AuthButton;

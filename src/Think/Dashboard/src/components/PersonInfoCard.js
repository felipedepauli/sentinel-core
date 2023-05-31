import React from 'react';
import { Card } from 'antd';

const PersonInfoCard = ({ personInfo }) => {
  return (
    <Card className="program_desc__block" title={`${personInfo.name} - ${personInfo.age}`}>
        {
            personInfo.status ?
            <p>Status: {personInfo.status === "active" ? "Sem BOs" : "Procurado pela Interpol"}</p>
            :
            <div></div>
        }
        <p>{personInfo.description}</p>
    </Card>
  );
};

export default PersonInfoCard;

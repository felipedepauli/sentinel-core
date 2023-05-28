import React from 'react';
import { 
    TbDrone,
    TbDroneOff
  } from "react-icons/tb";
import {
  BsFillArrowUpCircleFill,
  BsFillArrowLeftCircleFill,
  BsFillArrowRightCircleFill,
  BsFillArrowDownCircleFill,
  BsPersonBoundingBox,
  BsStopCircleFill
} from 'react-icons/bs';

const iconMap = {
    BsFillArrowUpCircleFill:    <BsFillArrowUpCircleFill    size={27}/>,
    BsFillArrowLeftCircleFill:  <BsFillArrowLeftCircleFill  size={27}/>,
    BsFillArrowRightCircleFill: <BsFillArrowRightCircleFill size={27}/>,
    BsFillArrowDownCircleFill:  <BsFillArrowDownCircleFill  size={27}/>,
    BsPersonBoundingBox:        <BsPersonBoundingBox        size={27}/>,
    TbDrone:                    <TbDrone                    size={27}/>,
    TbDroneOff:                 <TbDroneOff                 size={27}/>,
    BsStopCircleFill:           <BsStopCircleFill           size={27}/>
};

export default iconMap
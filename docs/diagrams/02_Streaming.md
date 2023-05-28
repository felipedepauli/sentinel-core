graph TB
  M["main.cpp"] -- "Initiates" --> W["WakeUP.cpp"]
  W -- "Listens and initiates" --> S["Session.cpp"]
  S -- "Requests frame" --> E["Eys.cpp"]
  E -- "Returns frame" --> S
  S -- "Sends frames" --> C["Cortx.py"]
  C -- "Displays and calls face recognition" --> A["Auth.py"]
  A -- "Performs face recognition and sends frames" --> C
  C -- "Sends frames" --> WS["Web Server"]
  WS -- "Sends frames" --> R["React App"]
  R -- "Displays frames and controls" --> U["User"]
  U -- "Sends drone controls" --> F["Flask API"]
  F -- "Has Drone class with flight control methods" --> D["Drone"]

  linkStyle 0 stroke:#2ecd71,stroke-width:2px;
  linkStyle 1 stroke:#2ecd71,stroke-width:2px;
  linkStyle 2 stroke:#2ecd71,stroke-width:2px;
  linkStyle 3 stroke:#2ecd71,stroke-width:2px;
  linkStyle 4 stroke:#2ecd71,stroke-width:2px;
  linkStyle 5 stroke:#2ecd71,stroke-width:2px;
  linkStyle 6 stroke:#2ecd71,stroke-width:2px;
  linkStyle 7 stroke:#2ecd71,stroke-width:2px;
  linkStyle 8 stroke:#2ecd71,stroke-width:2px;
  linkStyle 9 stroke:#2ecd71,stroke-width:2px;
  linkStyle 10 stroke:#2ecd71,stroke-width:2px;
  linkStyle 11 stroke:#2ecd71,stroke-width:2px;
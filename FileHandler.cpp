/**
*	@file FileHandler.cpp
*	@author Jonathon Greene
*	@date
*	@brief
*/

#include "FileHandler.h"

FileHandler::FileHandler(){}

FileHandler::~FileHandler(){}

bool FileHandler::openFile(vector<Event*> &calendar_param){
  // calendar = new std::vector<Event>();
  if(checkFile()){
    parseXML(calendar_param);
    return true;
  }
  return false;
}

// TODO finish attendee iteration printing
bool FileHandler::saveFile(vector<Event*> &calendar_param){
  ofstream myfile;
  myfile.open ("schedule.xml");
  myfile << "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\" ?>\n<Calendar>\n";
  for(int i = 0; i<calendar_param.size(); i++){
    string eventName = calendar_param[i]->getEventName();
    string eventCreator = calendar_param[i]->getEventCreator();
    string event_startTime = calendar_param[i]->getEventStartTime();
    string event_endTime = calendar_param[i]->getEventEndTime();
    string date = calendar_param[i]->getEventDate();
    // write to file
    myfile << "\t<Event eventName=\"" + eventName + "\" "
    + "eventCreator=\"" + eventCreator + "\" "
    + "eventStartTime=\"" + event_startTime + "\" "
    + "eventEndTime=\"" + event_endTime +"\" "
    + "date=\"" + date + "\"> ";

  // Interate over the attendees
  vector<vector<string>> attendees = calendar_param[i]->getAttendees();
    for(int j = 0; j<attendees.size(); j++){
    // for(int j = 0; j<2; j++){
      // cout << "attendees size: " << attendees.size() << endl;
      vector<string> individual_attendee = attendees[j];
      // cout << "individual_attendee node 1: " << individual_attendee[2] << endl;
      string name = individual_attendee[0];
      myfile << "\n\t\t<Attendee attendeeName=\"" + name + "\">\n";
      // TODO get number of seperate time slots per unique attendee
      for(int k = 1; k<individual_attendee.size(); k++)
      {
        // 0 index is name 1++ start times and add the 30 minutes
        string attendeeString_startTime = individual_attendee[k];
        int attendee_startTime = stoi(attendeeString_startTime);
        int attendee_stopTime = attendee_startTime;
        if(attendee_startTime%100>0){
          attendee_stopTime+=70;
        }
        else{
          attendee_stopTime+=30;
        }
        myfile << "\t\t\t<Time arriveTime=\"" + to_string(attendee_startTime) + "\" leaveTime=\""
          + to_string(attendee_stopTime) + "\"></Time>\n";
      }
    myfile << "\t\t</Attendee>";
    }
    myfile << "\n\t</Event>\n";
  }
  myfile << "</Calendar>";
  myfile.close();
  return true;
}

bool FileHandler::checkFile(){
  // Check if file exists. Create it and load with default text if it does not.
  if (!ifstream("schedule.xml"))
  {
    ofstream file("schedule.xml");
    if (!file)
    {
      //  File could not be created
      return false;
    }
    ofstream myfile;
    myfile.open ("schedule.xml");
    myfile << "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\" ?>\n<Calendar>\n</Calendar>";
    myfile.close();
  }
  return true;
}

void FileHandler::parseXML(vector<Event*> &calendar_param){
  xml_document<> doc;
	xml_node<> * root_node;
	// Read the xml file into a vector
	ifstream theFile ("schedule.xml");
	vector<char> buffer((istreambuf_iterator<char>(theFile)), istreambuf_iterator<char>());
	buffer.push_back('\0');
	// Parse the buffer using the xml file parsing library into doc
	doc.parse<0>(&buffer[0]);
	// Find our root node
	root_node = doc.first_node("Calendar");
	// Iterate over the events
  int size = 0;
	for (xml_node<> * event_node = root_node->first_node("Event");event_node;event_node = event_node->next_sibling())
	{cout << "\nevents" << endl;
		string eventName = event_node->first_attribute("eventName")->value();
		string eventCreator = event_node->first_attribute("eventCreator")->value();
    string startTime = event_node->first_attribute("eventStartTime")->value();
    string endTime = event_node->first_attribute("eventEndTime")->value();
    string date = event_node->first_attribute("date")->value();
    Event* toAdd =  new Event(eventName, eventCreator, startTime, endTime, date);
    calendar_param.push_back(toAdd);
		// Interate over the attendees
		for(xml_node<> * attendee_node = event_node->first_node("Attendee");attendee_node;attendee_node = attendee_node->next_sibling())
		{
			string attendeeName = attendee_node->first_attribute("attendeeName")->value();
			for(xml_node<> * attendeeTime_node = attendee_node->first_node("Time");
				attendeeTime_node;
				attendeeTime_node = attendeeTime_node->next_sibling())
			{
				string attendee_arriveTime = attendeeTime_node->first_attribute("arriveTime")->value();
				string attendee_leaveTime = attendeeTime_node->first_attribute("leaveTime")->value();
				calendar_param[size]->addAttendee(attendeeName,attendee_arriveTime,attendee_leaveTime);
			}
		}
    size++;
	}

}

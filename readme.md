#Talky

Talky is a simple network protocol designed that sits on top of TCP network transmissions, and is designed to work across platforms.

The main feature of Talky is that it trivially serialises any binary class using a simple << operator.

This means that no extra serialisation/deserialisation code is required, except for when the class holds pointers. In this case, look into extending TalkySerialisable.

##TODO:
What happens when there's a buffer overrun in the buffer when inputting from platform in rxClient/rxServer? (even though this is very++ unlikely to happen)

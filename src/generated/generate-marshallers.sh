qdbusxml2cpp -i marshallers.h -p socket_proxy -c SocketProxy ../../xml/Socket.xml
qdbusxml2cpp -i marshallers.h -p dec_proxy    -c DeviceEventControllerProxy    ../../xml/DeviceEventController.xml

qdbusxml2cpp -i marshallers.h -a accessible_adaptor    ../../xml/Accessible.xml
qdbusxml2cpp -i marshallers.h -a action_adaptor        ../../xml/Action.xml
qdbusxml2cpp -i marshallers.h -a application_adaptor   ../../xml/Application.xml
qdbusxml2cpp -i marshallers.h -a cache_adaptor         ../../xml/Cache.xml
qdbusxml2cpp -i marshallers.h -a component_adaptor     ../../xml/Component.xml
qdbusxml2cpp -i marshallers.h -a editable_text_adaptor ../../xml/EditableText.xml
qdbusxml2cpp -i marshallers.h -a event_adaptor         ../../xml/Event.xml
qdbusxml2cpp -i adaptor.h -a table_adaptor -l QSpiAdaptor ../../xml/Table.xml
qdbusxml2cpp -i adaptor.h -a text_adaptor -l QSpiAdaptor  ../../xml/Text.xml
qdbusxml2cpp -i marshallers.h -a value_adaptor         ../../xml/Value.xml

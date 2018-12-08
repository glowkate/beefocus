
#include <iostream>
#include <memory>
#include <unistd.h>

#include "focuser_state.h"
#include "hardware_interface.h"

std::unique_ptr<FOCUSER_STATE> focuser;

class NetInterfaceSim: public NetInterface {
  public:

  void reset() override {}
  void setup( DebugInterface& debugLog ) override
  {
    debugLog << "Simulator Net Interface Init\n";
  }
  bool getString( WifiDebugOstream& log, std::string& input ) override
  {
    fd_set readfds;
    FD_ZERO(&readfds);

    struct timeval timeout;
    timeout.tv_sec = 0;
    timeout.tv_usec = 0;

    FD_SET(STDIN_FILENO, &readfds );
    if ( select(1, &readfds, nullptr, nullptr, &timeout ))
    {
      std::cin >> input;
      return true;
    }
    input = "";
    return false;
  }
  NetInterface& operator<<( char c )
  {
    return *this;
  }
};

class HWISim: public HWI
{
  public: 

  void PinMode( Pin pin, int state ) override
  {
    //std::cout << "Pin Mode " << HWI::pinNames.at(pin) << " set to state " << state << "\n";
  }
  void DigitalWrite( Pin pin, int state ) override
  {
    const std::string name = HWI::pinNames.at(pin);
    std::cout << "Digital Write Pin " << name << " state " << state << "\n";
  }
  int DigitalRead( Pin pin ) override
  {
    //std::cout << "Digital Read " << HWI::pinNames.at(pin) << " returning 0";
    return 0;
  }
};

class DebugInterfaceSim: public DebugInterface
{
  void rawWrite( const char* bytes, std::size_t numBytes ) override
  {
    for ( int i=0; i<numBytes; ++i )
    {
      std::cout << bytes[i];
    }
  }
};

void loop() {
  focuser->loop();
}

void setup() {
  std::unique_ptr<NetInterface> wifi( new NetInterfaceSim );
  std::unique_ptr<HWI> hardware( new HWISim );
  std::unique_ptr<DebugInterface> debug( new DebugInterfaceSim );
  focuser = std::unique_ptr<FOCUSER_STATE>(
     new FOCUSER_STATE( 
        std::move(wifi), 
        std::move(hardware),
				std::move(debug) )
  );
  focuser->setup();
}

int main(int argc, char* argv[])
{
  setup();
  for ( ;; ) 
  {
    loop();
  }
}


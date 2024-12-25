#include "DisplayList.h"

namespace Graphics::EVE
{
class CommandBuffer : public DisplayList
{
public:
	CommandBuffer() : DisplayList(EVE_CMDFIFO_SIZE)
	{
	}

#include "cpcmd.h"
};

} // namespace Graphics::EVE

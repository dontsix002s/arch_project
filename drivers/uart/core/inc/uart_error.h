#pragma once

namespace uart {

	enum class Error
	{
		Ok = 0,

		// Generic
		NotImplemented,
		InvalidArg,
		Busy,
		Timeout,

		// HW/line errors (future)
		Overrun,
		Framing,
		Parity,
		Noise,
	};

}  // namespace uart
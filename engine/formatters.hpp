# pragma once

# include "raylib.h"

// Custom fmt formatters


template <> struct fmt::formatter<Vector2> {
  char presentation = 'f';

  constexpr auto parse(format_parse_context& ctx) -> decltype(ctx.begin()) {
    // Parse the presentation format and store it in the formatter:
    auto it = ctx.begin(), end = ctx.end();
    if (it != end && (*it == 'f' || *it == 'e')) presentation = *it++;

    // Check if reached the end of the range:
    if (it != end && *it != '}') throw format_error("invalid format");

    // Return an iterator past the end of the parsed range:
    return it;
  }

  template <typename FormatContext>
  auto format(const Vector2& vec, FormatContext& ctx) const -> decltype(ctx.out()) {
    // ctx.out() is an output iterator to write to.
    return presentation == 'f'
              ? fmt::format_to(ctx.out(), "({:.1f}, {:.1f})", vec.x, vec.y)
              : fmt::format_to(ctx.out(), "({:.1e}, {:.1e})", vec.x, vec.y);
  }
};

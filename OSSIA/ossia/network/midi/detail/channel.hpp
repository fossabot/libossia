#pragma once
#include <array>
#include <cinttypes>
#include <utility>
namespace ossia
{
namespace net
{
namespace midi
{
using midi_size_t = uint8_t;
class midi_address;
struct midi_channel
{
  // [ note, vel ]
  std::pair<midi_size_t, midi_size_t> note_on{};
  std::pair<midi_size_t, midi_size_t> note_off{};

  // [ CC, value ]
  std::pair<midi_size_t, midi_size_t> cc{};

  // PC
  midi_size_t pc{};

  // velocity or value
  std::array<midi_size_t, 128> note_on_N = {{64}};
  std::array<midi_size_t, 128> note_off_N = {{64}};
  std::array<midi_size_t, 128> cc_N = {{64}};
  // No need to store PC since they are only impulses

  // Callbacks
  midi_address* callback_note_on{};
  midi_address* callback_note_off{};
  midi_address* callback_cc{};
  midi_address* callback_pc{};
  std::array<midi_address*, 128> callback_note_on_N = {{}};
  std::array<midi_address*, 128> callback_note_off_N = {{}};
  std::array<midi_address*, 128> callback_cc_N = {{}};
  std::array<midi_address*, 128> callback_pc_N = {{}};
};
}
}
}

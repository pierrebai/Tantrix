#pragma once

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers

#include "dak/six_eight/direction.h"
#include "dak/six_eight/position.h"

#include <algorithm>
#include <unordered_map>
#include <vector>


namespace dak::six_eight
{

   ////////////////////////////////////////////////////////////////////////////
   //
   // Description of a tile: position of the blocks in the tile.

   struct tile_description_t
   {
      int possible_rotations;
      position_t block_positions[6];
   };

   ////////////////////////////////////////////////////////////////////////////
   //
   // A rotated tile with its block positions.

   struct tile_t
   {
      using id_t = char;
      
      // Create a tile.
      tile_t() = default;
      tile_t(id_t an_id) : my_id(std::uint8_t(an_id)), my_rotation(0) {}

      // Return the tile id.
      id_t id() const { return my_id; }

      // Return the tile id of all tiles.
      static const std::vector<id_t>& get_all_ids();

      // Return the tile current rotation.
      int get_rotation() const { return my_rotation; }

      // Rotate the tile in place by a multiple of quarter of a turn.
      tile_t& rotate_in_place(int an_amount)
      {
         my_rotation = (my_rotation + an_amount) % get_description().possible_rotations;
         return *this;
      }

      // Rotate the tile by a multiple of sixth of a turn and return a new tile.
      tile_t rotate(int an_amount) const
      {
         return tile_t(*this).rotate_in_place(an_amount);
      }

      // Check if the tile are the same tile, ignoring orientation.
      bool is_same(const tile_t& an_other) const
      {
         return my_id == an_other.my_id;
      }

      // Check if the tile is valid.
      bool is_valid() const { return my_id != 0; }

      // Tile comparison. The same tile with different orientations will be different.
      auto operator<=>(const tile_t& an_other) const = default;

      // Return the six block positions occupied by the rotated tile.
      const tile_description_t& get_description() const;

   private:
      id_t         my_id = 0;
      std::uint8_t my_rotation = 0;
   };
}


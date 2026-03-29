#pragma once

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers

#include <dak/solver/solution.h>

#include "dak/six_eight/position.h"
#include "dak/six_eight/tile.h"

#include <set>
#include <vector>
#include <optional>

namespace dak::six_eight
{
   struct placed_tile_t : solver::solution_part_t
   {
      tile_t      tile;
      position_t  pos;

      placed_tile_t() = default;
      placed_tile_t(tile_t a_tile, position_t a_pos)
      : tile(std::move(a_tile)), pos(std::move(a_pos)) {}

      static std::shared_ptr<placed_tile_t> make()
      { return std::make_shared<placed_tile_t>(); }

      static std::shared_ptr<placed_tile_t> make(tile_t a_tile, position_t a_pos)
      { return std::make_shared<placed_tile_t>(std::move(a_tile), std::move(a_pos)); }
   
      // Compare placed tiles.
      auto operator<=>(const placed_tile_t&) const = default;
   };

   ////////////////////////////////////////////////////////////////////////////
   //
   // Solution that places all the given tiles.

   struct solution_t : solver::solution_t
   {
      using tiles_by_pos_t = placed_tile_t[8];

      // Create a new solution.
      solution_t();
      solution_t(const tile_t& a_tile, const position_t& a_pos)
      : solution_t()
      {
         add_tile(a_tile, a_pos);
      }

      // Make a copy of this solution.
      ptr_t clone() const override { return std::make_shared<six_eight::solution_t>(*this); }

      // Access the solution tiles.
      const placed_tile_t* tiles() const { return my_tiles; }
      size_t tiles_count() const { return my_tiles_count; }

      const tile_t& tile_at(int x, int y) const { return tile_at(position_t(x, y)); }
      const tile_t& tile_at(const position_t& a_pos) const { return internal_tile_at(a_pos); }

      tile_t& tile_at(int x, int y) { return tile_at(position_t(x, y)); }
      tile_t& tile_at(const position_t& a_pos) { return internal_tile_at(a_pos); }

      // Add a tile to the solution.
      void add_tile(const tile_t& a_tile, const position_t& a_pos);

      // Add a part of the solution to this solution.
      void add_part(const solver::solution_part_t::ptr_t& a_part) override;

      // Normalize the solution so that any other identical solutions will
      // have the same set of positions and orientation.
      void normalize();

      // Check if a position is already occupied.
      bool is_occupied(const position_t& a_pos) const;

      // Get the next position to fill.
      position_t get_next_position_to_fill() const;

      // Check if a tile at a given position would be compatible with the solution.
      bool is_compatible(const tile_t& a_tile, const position_t& a_pos) const;
      bool is_compatible(const solver::solution_part_t::ptr_t& a_part) const override;

      // Check if the solution is valid.
      bool is_valid() const override;

      // Nearing completion, stop spawning threads.
      bool is_almost_done(const std::shared_ptr<solver::problem_t>& a_problem) const override { return my_tiles_count > 5; }

      // Compare solutions.
      std::strong_ordering operator<=>(const solver::solution_t& another_solution) const override;
      std::strong_ordering operator<=>(const six_eight::solution_t& another_solution) const;
      bool operator==(const solution_t& another_solution) const { return operator<=>(another_solution) == 0; }

      // Add a similar solution to this solution.
      void add_similar_solution(const solver::solution_t::ptr_t& another_solution) override;

   private:
      tile_t& internal_tile_at(const position_t& a_pos) const;

      size_t         my_tiles_count = 0;
      tiles_by_pos_t my_tiles;
      tile_t::id_t   my_tiles_at_pos[6][8] = { 0 };
   };

   using all_solutions_t = std::set<solution_t>;
}


#pragma once

#include "Math.h"

#include <algorithm>
#include <array>
#include <functional>

namespace GravityFun
{
    template <int SizeX, int SizeY, int AreaX, int AreaY, int CellCapacity>
    class ObjectMapper
    {
    private:
        static constexpr double PositionToIndexX = (double)SizeX / AreaX;
        static constexpr double PositionToIndexY = (double)SizeY / AreaY;
        static constexpr int IndexMultiplierX = SizeY * CellCapacity;
        static constexpr int IndexMultiplierY = CellCapacity;

        static constexpr int SlotsCount = SizeX * SizeY;
        static constexpr double SlotSizeX = (double)AreaX / SizeX;
        static constexpr double SlotSizeY = (double)AreaY / SizeY;

        static constexpr int MaxLeft = (SizeX / 2);
        static constexpr int MaxRight = SizeX - MaxLeft - 1;
        static constexpr int MaxBottom = (SizeY / 2);
        static constexpr int MaxTop = SizeY - MaxBottom - 1;

        std::array<int, SizeX * SizeY * CellCapacity> MappedObjectBuffer;

        inline int GetIndexX(double position_x) const
        {
            return (int)(position_x * PositionToIndexX);
        }

        inline int GetIndexY(double position_y) const
        {
            return (int)(position_y * PositionToIndexY);
        }

        inline int GetIndex(int index_x, int index_y) const
        {
            int x = index_x % SizeX;
            if (x < 0)
                x += SizeX;
            int y = index_y % SizeY;
            if (y < 0)
                y += SizeY;
            return x * IndexMultiplierX + y * IndexMultiplierY;
        }

        inline int GetIndex(const Math::Vec2& position) const
        {
            int x = GetIndexX(position.x);
            int y = GetIndexY(position.y);
            return GetIndex(x, y);
        }

        template <int MaxCount>
        inline int GetObjects(int start, std::array<int, MaxCount>& result, int i = 0) const
        {
            int stop = start + SlotsCount;
            for (int j = start; j < stop && i < MaxCount; j++)
            {
                if (MappedObjectBuffer[j] != -1)
                {
                    result[i] = MappedObjectBuffer[j];
                    i++;
                }
                else
                {
                    return i;
                }
            }
            return i;
        }

        inline bool VisitObjects(int start, const std::function<bool(int)>& visitor) const
        {
            int stop = start + SlotsCount;
            for (int j = start; j < stop; j++)
            {
                if (MappedObjectBuffer[j] != -1)
                {
                    if (visitor(MappedObjectBuffer[j]))
                        return true;
                }
                else
                {
                    return false;
                }
            }
            return false;
        }
    public:
        inline void Clear()
        {
            for (int i = 0; i < SlotsCount; i++)
            {
                MappedObjectBuffer[i * CellCapacity] = -1;
            }
        }

        inline void AddObject(Math::Vec2 position, int object_index)
        {
            int start = GetIndex(position);
            int stop = start + SlotsCount;
            for (int i = start; i < stop; i++)
            {
                if (MappedObjectBuffer[i] == -1)
                {
                    MappedObjectBuffer[i] = object_index;
                    if (++i < stop)
                    {
                        MappedObjectBuffer[i] = -1;
                    }
                    return;
                }
            }
            stop = 0;
        }

        /// @brief Fills the result array with the objects in the slot where the position is in.
        /// @param i The initial index to write to in results.
        /// @return The final value of i, which is the number of objects if parameter i=0 is given.
        template <int MaxCount>
        inline int GetObjects(Math::Vec2 position, std::array<int, MaxCount>& result, int i = 0) const
        {
            int start = GetIndex(position);
            return GetObjects<MaxCount>(start, result, i);
        }

        /// @brief Visits the objects in the slot where the position is in.
        ///        The visitor can return true to stop visiting any more objects.
        /// @return Whether the visitor returned true to stop visiting.
        template <int MaxCount>
        inline bool VisitObjects(Math::Vec2 position, const std::function<bool(int)>& visitor) const
        {
            int start = GetIndex(position);
            return VisitObjects<MaxCount>(start, visitor);
        }

        /// @brief Visits the objects in the slots in proximity of the position.
        ///        The visitor can return true to stop visiting any more objects.
        /// @return Whether the visitor returned true to stop visiting.
        inline bool VisitObjects(Math::Vec2 position, double radius, const std::function<bool(int)>& visitor) const
        {
            // Center position
            int center_x = GetIndexX(position.x);
            int center_y = GetIndexY(position.y);

            // Area from center
            int left = center_x - GetIndexX(position.x - radius);
            int right = GetIndexX(position.x + radius) - center_x;
            if (left + right >= SizeX) // Horizontal area overlap
            {
                left = MaxLeft;
                right = MaxRight;
            }
            int bottom = center_y - GetIndexY(position.y - radius);
            int top = GetIndexY(position.y + radius) - center_y;
            if (bottom + top >= SizeY) // Vertical area overlap
            {
                bottom = MaxBottom;
                top = MaxTop;
            }
            int index_radius = std::max({ left, right, bottom, top });

            // Start from center
            if (VisitObjects(GetIndex(center_x, center_y), visitor))
                return true;
            for (int distance = 1; distance <= index_radius; distance++)
            {
                bool visit_bottom = distance <= bottom;
                bool visit_right = distance <= right;
                bool visit_top = distance <= top;
                bool visit_left = distance <= left;
                int l = center_x - std::min(distance, left);
                int r = center_x + std::min(distance, right);
                int b = center_y - std::min(distance, bottom);
                int t = center_y + std::min(distance, top);
                // Bottom
                if (visit_bottom)
                {
                    int y = center_y - distance;
                    for (int x = l; visit_right ? x < r : x <= r; x++) // Left to right
                    {
                        if (VisitObjects(GetIndex(x, y), visitor))
                            return true;
                    }
                }
                // Right
                if (visit_right)
                {
                    int x = center_x + distance;
                    for (int y = b; visit_top ? y < t : y <= t; y++) // Bottom to top
                    {
                        if (VisitObjects(GetIndex(x, y), visitor))
                            return true;
                    }
                }
                // Top
                if (visit_top)
                {
                    int y = center_y + distance;
                    for (int x = r; visit_left ? x > l : x >= l; x--) // Right to left
                    {
                        if (VisitObjects(GetIndex(x, y), visitor))
                            return true;
                    }
                }
                // Left
                if (visit_left)
                {
                    int x = center_x - distance;
                    for (int y = t; visit_bottom ? y > b : y >= b; y--) // Top to bottom
                    {
                        if (VisitObjects(GetIndex(x, y), visitor))
                            return true;
                    }
                }
            }
            return false;
        }
    };
}

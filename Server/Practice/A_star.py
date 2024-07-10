import heapq
import random

def create_grid(size=10, obstacle_prob=0.3):
    return [[1 if random.random() < obstacle_prob else 0 for _ in range(size)] for _ in range(size)]


def heuristic(a, b):
    return abs(b[0] - a[0]) + abs(b[1] - a[1])


def get_neighbors(grid, node):
    directions = [(0, 1), (1, 0), (0, -1), (-1, 0)]
    result = []
    for direction in directions:
        neighbor = (node[0] + direction[0], node[1] + direction[1])
        if 0 <= neighbor[0] < len(grid) and 0 <= neighbor[1] < len(grid[0]) and grid[neighbor[0]][neighbor[1]] != 1:
            result.append(neighbor)
    return result


def a_star(grid, start, goal):
    heap = [(0, start)]
    came_from = {}
    g_score = {start: 0}
    f_score = {start: heuristic(start, goal)}

    while heap:
        current = heapq.heappop(heap)[1]

        if current == goal:
            path = []
            while current in came_from:
                path.append(current)
                current = came_from[current]
            path.append(start)
            return path[::-1]

        for neighbor in get_neighbors(grid, current):
            tentative_g_score = g_score[current] + 1

            if neighbor not in g_score or tentative_g_score < g_score[neighbor]:
                came_from[neighbor] = current
                g_score[neighbor] = tentative_g_score
                f_score[neighbor] = g_score[neighbor] + heuristic(neighbor, goal)
                heapq.heappush(heap, (f_score[neighbor], neighbor))

    return None


# 예시 사용
grid = create_grid(10)

start = (0, 0)
goal = (9, 9)
grid[start[0]][start[1]] = 0
grid[goal[0]][goal[1]] = 0

# 그리드 출력
for row in grid:
    print(' '.join(map(str, row)))

path = a_star(grid, start, goal)
if path:
    print("\n찾은 경로:", path)
else:
    print("\n경로를 찾을 수 없습니다.")
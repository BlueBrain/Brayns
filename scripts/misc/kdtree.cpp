#include <algorithm>
#include <array>
#include <cmath>
#include <iostream>
#include <limits>
#include <stdexcept>
#include <vector>

using Point = std::array<float, 3>;

auto none = std::numeric_limits<std::size_t>::max();

struct Node
{
    Point point = {0, 0, 0};
    std::size_t left = none;
    std::size_t right = none;

    Node() = default;

    explicit Node(const Point &value)
        : point(value)
    {
    }
};

struct KdTree
{
    std::size_t root = none;
    std::vector<Node> nodes;
};

struct NearestPoint
{
    std::size_t index = none;
    float distance = 0.0f;
};

struct SearchContext
{
    std::size_t index;
    std::size_t axis = 0;
    NearestPoint result;

    explicit SearchContext(std::size_t root)
        : index(root)
    {
    }
};

std::size_t nextAxis(std::size_t axis)
{
    static constexpr auto dimensions = Point().size();
    return (axis + 1) % dimensions;
}

std::size_t buildTree(std::vector<Node> &nodes, std::size_t first, std::size_t last, std::size_t axis)
{
    if (last <= first)
    {
        return none;
    }

    auto pivot = first + (last - first) / 2;
    auto begin = nodes.begin();

    std::nth_element(
        begin + first,
        begin + pivot,
        begin + last,
        [=](auto &left, auto &right) { return left.point[axis] < right.point[axis]; });

    axis = nextAxis(axis);

    nodes[pivot].left = buildTree(nodes, first, pivot, axis);
    nodes[pivot].right = buildTree(nodes, pivot + 1, last, axis);

    return pivot;
}

std::size_t buildTree(std::vector<Node> &nodes)
{
    auto first = 0;
    auto last = nodes.size();
    auto axis = 0;
    return buildTree(nodes, first, last, axis);
}

KdTree createTree(const std::vector<Point> &points)
{
    auto nodes = std::vector<Node>(points.begin(), points.end());
    auto root = buildTree(nodes);
    return {root, std::move(nodes)};
}

float computeDistance(const Point &a, const Point &b)
{
    auto distance = 0.0f;
    for (auto i = std::size_t(0); i < a.size(); ++i)
    {
        auto d = a[i] - b[i];
        distance += d * d;
    }
    return std::sqrt(distance);
}

void findNearest(SearchContext &context, const std::vector<Node> &nodes, const Point &point)
{
    auto &index = context.index;
    if (index == none)
    {
        return;
    }

    auto &node = nodes[index];
    auto &position = node.point;
    auto distance = computeDistance(position, point);

    auto &result = context.result;
    if (result.index == none || distance < result.distance)
    {
        result.index = index;
        result.distance = distance;
    }

    if (distance == 0)
    {
        return;
    }

    auto &axis = context.axis;
    distance = position[axis] - point[axis];

    axis = nextAxis(axis);

    auto left = distance > 0;
    index = left ? node.left : node.right;
    findNearest(context, nodes, point);

    if (distance * distance >= result.distance)
    {
        return;
    }

    index = left ? node.right : node.left;
    findNearest(context, nodes, point);
}

NearestPoint findNearest(const KdTree &tree, const Point &point)
{
    auto context = SearchContext(tree.root);
    findNearest(context, tree.nodes, point);
    return context.result;
}

const Point &findNearestPoint(const KdTree &tree, const Point &point)
{
    if (tree.nodes.empty() || tree.root == none)
    {
        throw std::runtime_error("Invalid tree");
    }
    auto result = findNearest(tree, point);
    if (result.index == none)
    {
        throw std::runtime_error("Unexpected search failure");
    }
    return tree.nodes[result.index].point;
}

int main()
{
    auto points = std::vector<Point>({
        {0, 0, 0},
        {0, 1, 0},
        {0, 0, 1},
        {0, 0, 3},
        {1, 1, 1},
    });

    auto tree = createTree(points);

    for (const auto &point : points)
    {
        auto test = findNearestPoint(tree, point);
        if (test != point)
        {
            std::cout << "Failed !!!\n";
            return -1;
        }
    }

    auto test = findNearestPoint(tree, {0, 0.8f, 0});

    if (test != Point{0, 1, 0})
    {
        std::cout << "Failed !!!\n";
        return -1;
    }

    test = findNearestPoint(tree, {0.9f, 0.8f, 0.6f});

    if (test != Point{1, 1, 1})
    {
        std::cout << "Failed !!!\n";
        return -1;
    }

    return 0;
}

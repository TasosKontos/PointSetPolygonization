#include <string.h>
#include <iostream>
#include <sstream>
#include <vector>
#include <numeric>
#include <fstream>
#include <random>
#include <CGAL/convex_hull_2.h>
#include <CGAL/Convex_hull_traits_adapter_2.h>
#include <CGAL/property_map.h>
#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/Polygon_2.h>
#include <CGAL/intersections.h>
typedef CGAL::Exact_predicates_inexact_constructions_kernel K;
typedef K::Point_2      Point_2;
typedef CGAL::Polygon_2<K> Polygon_2;
typedef K::Segment_2    Segment_2;
typedef CGAL::Convex_hull_traits_adapter_2<K,CGAL::Pointer_property_map<Point_2>::type > Convex_hull_traits_2;

using namespace std;
/*--------------------------------------------------------------------------------------------------------------------*/
void printPointSet(vector<Point_2>);
void printUserInput(string, string, string, string, string);
void printErrorPointNotFound(Point_2, vector<Point_2>);
vector<Segment_2> getPolygonEdgesFromPoints(vector<Point_2>);
Polygon_2 getSimplePolygonFromPoints(vector<Point_2>);
vector<Point_2> ProcessInputFile(string);
bool comparePointXdesc(Point_2, Point_2);
bool comparePointXasc(Point_2, Point_2);
bool comparePointYdesc(Point_2, Point_2);
bool comparePointYasc(Point_2, Point_2);
int findIndexOfPointInPointSet(vector<Point_2>, Point_2);
vector<Point_2> sortPointset(vector<Point_2>, string);
vector<Point_2> createConvexHull(vector<Point_2>);
vector<Segment_2> getRedEdges(vector<Point_2>, vector<Point_2>, Point_2);
vector<Segment_2> findChainOfEdges(Point_2, Point_2, vector<Point_2>);
bool isEdgeVisibleFromPoint(Point_2, Segment_2, vector<Segment_2>);
vector<Segment_2> calculateVisibleEdges(vector<Segment_2>, vector<Point_2>, Point_2, vector<Point_2>);
Segment_2 randomEdgeSelection(vector<Segment_2>);
Segment_2 minAreaEdgeSelection(vector<Segment_2>, Point_2, vector<Point_2>);
Segment_2 maxAreaEdgeSelection(vector<Segment_2>, Point_2, vector<Point_2>);
Segment_2 getPolygonEdgeToReplace(vector<Segment_2>, Point_2, vector<Point_2>, int);
vector<Point_2> insertPointToPolygonPointSet(Point_2, Segment_2, vector<Point_2>);
vector<Point_2> IncrementalAlg(vector<Point_2>, int, string);
/*--------------------------------------------------------------------------------------------------------------------*/

void printPointSet(vector<Point_2> pointSet) {
    for(int i=0 ; i < pointSet.size() ; ++i) {
        cout << to_string(pointSet[i].x()) + " " + to_string(pointSet[i].y()) << endl;
    }
}

void printUserInput(string inputFileName, string outputFileName, string algorithmName, string edgeSelection, string incrementalInit) {
    cout << endl << "User input:" << endl;
    cout << "input file: " + inputFileName << endl;
    cout << "output file: " + outputFileName << endl;
    cout << "algorithm: " + algorithmName << endl;
    cout << "edge selection: " + edgeSelection << endl;
    cout << "incremental initialization: " + incrementalInit << endl << endl;
}

void printErrorPointNotFound(Point_2 point, vector<Point_2> polygon) {
    cout << endl;
    cout << point.x() << " " << point.y() << endl;
    cout << "^This *Point* was not found in vector!" << endl;
    printPointSet(polygon);
    exit(-1);
}

vector<Segment_2> getPolygonEdgesFromPoints(vector<Point_2> pointSet) {
    vector<Segment_2> polygon;
    for(int i=0 ; i<pointSet.size() ; ++i){
        (i != (pointSet.size()-1)) ? polygon.push_back(Segment_2(pointSet[i],pointSet[i+1])) : polygon.push_back(Segment_2(pointSet[i],pointSet[0]));
    }
    return polygon;
}

Polygon_2 getSimplePolygonFromPoints(vector<Point_2> pointSet) {
    Polygon_2 polygon;
    for(int i=0 ; i<pointSet.size() ; ++i)
        polygon.push_back(pointSet[i]);
    if (polygon.is_simple()) {
        return polygon;
    } else {
        cout << endl << "Not a simple polygon!" << endl;
        exit(-1);
    }
    return polygon;
}

vector<Point_2> ProcessInputFile(string file_name) {
    ifstream file(file_name);
    vector<Point_2> pointSet;
    int lineIterator, xCordinate, yCordinate;
    string garbageInfo;
    getline(file, garbageInfo);
    getline(file, garbageInfo);
    while (file >> lineIterator >> xCordinate >> yCordinate)
        pointSet.push_back(Point_2(xCordinate, yCordinate));
    return pointSet;
}

bool comparePointXdesc(Point_2 A, Point_2 B) {
    return A.x()>B.x();
}
bool comparePointXasc(Point_2 A, Point_2 B) {
    return A.x()<B.x();
}
bool comparePointYdesc(Point_2 A, Point_2 B) {
    return A.y()>B.y();
}
bool comparePointYasc(Point_2 A, Point_2 B) {
    return A.y()<B.y();
}

int findIndexOfPointInPointSet(vector<Point_2> pointSet, Point_2 point) {
    int index = -1;
    for(int i=0 ; i<pointSet.size() ; ++i) {
        if( pointSet[i].x() == point.x() && pointSet[i].y() == point.y() ) {
            index = i;
            break;
        }
    }
    return index;
}

vector<Point_2> sortPointset(vector<Point_2> pointSet, string sortMethod) {
    if (sortMethod == "1a")
        std::sort(pointSet.begin(), pointSet.end(), comparePointXdesc);
    else if (sortMethod == "1b")
        std::sort(pointSet.begin(), pointSet.end(), comparePointXasc);
    else if (sortMethod == "2a")
        std::sort(pointSet.begin(), pointSet.end(), comparePointYdesc);
    else if (sortMethod == "2b")
        std::sort(pointSet.begin(), pointSet.end(), comparePointYasc);
    else {
        cout << "Invalid sort method." << endl;
        exit(-1);
    }
    return pointSet;
}

vector<Point_2> createConvexHull(vector<Point_2> pointSet) {
    vector<Point_2> convexHull;
    std::vector<std::size_t> indices(pointSet.size()), out;
    std::iota(indices.begin(), indices.end(),0);
    CGAL::convex_hull_2(indices.begin(), indices.end(), std::back_inserter(out),
                        Convex_hull_traits_2(CGAL::make_property_map(pointSet)));
    for( std::size_t i : out )
        convexHull.push_back(pointSet[i]);
    return convexHull;
}

bool isPointOnCHPerimeter(Point_2 point, vector<Point_2> convex_hull, vector<Segment_2> edges){
    for(int i=0; i<(convex_hull.size()-2); i++){
        Segment_2 currentEdge(convex_hull[i], convex_hull[i+1]);
        if(do_intersect(currentEdge,point)){
            edges.push_back(currentEdge);
            //cout<< "Found edge on CH perimeter ["<<currentEdge.start().x()<<" ,"<<currentEdge.start().y()<<"]["<<currentEdge.end().x()<<" ,"<<currentEdge.end().y()<<"]"<<endl;
            return true;
        }
    }

    Segment_2 lastSegment(convex_hull.back(), convex_hull.front());
    if(do_intersect(lastSegment, point)){
        edges.push_back(lastSegment);
        //cout<< "Found edge on CH perimeter ["<<lastSegment.start().x()<<" ,"<<lastSegment.start().y()<<"]["<<lastSegment.end().x()<<" ,"<<lastSegment.end().y()<<"]"<<endl;
        return true;
    }
    return false;
}

vector<Segment_2> getRedEdges(vector<Point_2> oldConvexHull, vector<Point_2> newConvexHull, Point_2 newPoint){
    vector<Segment_2> replacedEdges;
    int positionOfNewPoint = findIndexOfPointInPointSet(newConvexHull,newPoint);
    Point_2 upperPoint, lowerPoint;

    (positionOfNewPoint == (newConvexHull.size()-1)) ? (lowerPoint = newConvexHull[0]) : (lowerPoint = newConvexHull[positionOfNewPoint+1]);
    (positionOfNewPoint == 0) ? (upperPoint = newConvexHull[newConvexHull.size()-1]) : (upperPoint = newConvexHull[1]);

    if(isPointOnCHPerimeter(newPoint, oldConvexHull, replacedEdges)) return replacedEdges;

    vector<Segment_2> oldConvexHullPolygon = getPolygonEdgesFromPoints(oldConvexHull);
    vector<Segment_2> newConvexHullPolygon = getPolygonEdgesFromPoints(newConvexHull);
    for(Segment_2 oldConvexEdge: oldConvexHullPolygon) {
        bool foundEdge = false;
        for(Segment_2 newConvexEdge: newConvexHullPolygon) {
            if (oldConvexEdge==newConvexEdge) {
                foundEdge = true;
                break;
            }
        }
        if(!foundEdge) {
            replacedEdges.push_back(oldConvexEdge);
            //cout<<"Pushed back Red Edge ["<<oldConvexEdge.start().x()<<" ,"<<oldConvexEdge.start().y()<<"]["<<oldConvexEdge.end().x()<<" ,"<<oldConvexEdge.end().y()<<"]"<<endl;
        }
    }
    return replacedEdges;

    /*int positionOfUpperElement = findIndexOfPointInPointSet(oldConvexHull, upperPoint);

    bool isLowerElement = false;
    int index = positionOfUpperElement;
    while(!isLowerElement){
        Point_2 segmentPointA, segmentPointB;
        segmentPointA = oldConvexHull[index];
        (index == (oldConvexHull.size()-1)) ? (index = 0) : (++index);
        segmentPointB = oldConvexHull[index];
        replacedEdges.push_back(Segment_2(segmentPointA, segmentPointB));
        if (segmentPointB==lowerPoint)
            isLowerElement=true;
    }
    return replacedEdges;*/
}
vector<Segment_2> findChainOfEdges(Point_2 pointA, Point_2 pointB, vector<Point_2> polygon) {
    vector<Segment_2> chainedEdges;
    auto indexOfPointA = find(polygon.begin(), polygon.end(), pointA);
    if (indexOfPointA==polygon.end()) {
        printErrorPointNotFound(pointA, polygon);
    }
    auto indexOfPointB = find(polygon.begin(), polygon.end(), pointB);
    if (indexOfPointB==polygon.end()) {
        printErrorPointNotFound(pointB, polygon);
    }
    int indexA = indexOfPointA - polygon.begin();
    int indexB = indexOfPointB - polygon.begin();
    int startingIndex, endingIndex;
    if(indexA>indexB) {
        startingIndex = indexB;
        endingIndex = indexA;
    } else {
        startingIndex = indexA;
        endingIndex = indexB;
    }
    for(int i=startingIndex ; i<endingIndex ; ++i){
        chainedEdges.push_back(Segment_2(polygon[i],polygon[i+1]));
    }
    return chainedEdges;
}

bool segmentIntersectsPolygonEdge(Segment_2 seg, Segment_2 polygonEdge){
    const auto result = intersection(seg, polygonEdge);
    if(result){
        if (const Segment_2* s = boost::get<Segment_2>(&*result)) {
            return false;
        } else {
            const Point_2* p = boost::get<Point_2 >(&*result);
            if(!(*p==seg.end())) return true;
        }
    }
    return false;
}

bool isEdgeVisibleFromPoint(Point_2 point, Segment_2 edge, vector<Segment_2> polygon){
    Point_2 edgeMidpoint((edge.start().x()+edge.end().x())/2, (edge.start().y()+edge.end().y())/2);
    Segment_2 edgeA(point, edge[0]), edgeB(point, edge[1]), edgeMid(point, edgeMidpoint);
    for(Segment_2 polygonEdge : polygon){
        if(segmentIntersectsPolygonEdge(edgeA, polygonEdge) || segmentIntersectsPolygonEdge(edgeB, polygonEdge) ||
                segmentIntersectsPolygonEdge(edgeMid, polygonEdge)) return false;
    }
    return true;

}

bool segmentsEquivalent(Segment_2 A, Segment_2 B){
    return((A.start()==B.end() && A.end()==B.start())||A==B);
}

vector<Segment_2> calculateVisibleEdges(vector<Segment_2> redEdges, vector<Point_2> polygonPointSet, Point_2 added_point, vector<Point_2> convexHull){
    vector<Segment_2> visibleEdges;
    vector<Segment_2> polygonEdges = getPolygonEdgesFromPoints(polygonPointSet);
    for(Segment_2 redEdge : redEdges){
        bool foundEdge=false;
        for(Segment_2 polygonEdge : polygonEdges){
            if(segmentsEquivalent(redEdge, polygonEdge)) {
                visibleEdges.push_back(redEdge);
                //cout<<"Found Visible Edge ["<<redEdge.start().x()<<" ,"<<redEdge.start().y()<<"]["<<redEdge.end().x()<<" ,"<<redEdge.end().y()<<"]"<<endl;
                foundEdge = true;
                break;
            }
        }
        if(!foundEdge) {
            vector<Segment_2> chainedEdges = findChainOfEdges(redEdge.start(),redEdge.end(),polygonPointSet);
            for(Segment_2 chainedEdge: chainedEdges){
                if(isEdgeVisibleFromPoint(added_point,chainedEdge,polygonEdges)) {
                    visibleEdges.push_back(chainedEdge);
                    //cout<<"Found Visible Edge ["<<chainedEdge.start().x()<<" ,"<<chainedEdge.start().y()<<"]["<<chainedEdge.end().x()<<" ,"<<chainedEdge.end().y()<<"]"<<endl;
                }
            }
        }
    }
    return visibleEdges;
}

Segment_2 randomEdgeSelection(vector<Segment_2> replaceableEdges) {
    std::random_device randomDevice;
    std::mt19937 gen(randomDevice());
    std::uniform_int_distribution<> distr(0, replaceableEdges.size()-1);
    return replaceableEdges[distr(gen)];
}

Segment_2 minAreaEdgeSelection(vector<Segment_2> replaceableEdges, Point_2 currentPoint, vector<Point_2> polygonPointSet) {
    Segment_2 edgeToReturn = replaceableEdges[0];
    vector<Point_2> currentPolygonPointSet = insertPointToPolygonPointSet(currentPoint,edgeToReturn,polygonPointSet);
    double minArea = getSimplePolygonFromPoints(currentPolygonPointSet).area();
    for(int i=1 ; i<replaceableEdges.size() ; ++i) {
        Segment_2 tempEdgeToReturn = replaceableEdges[i];
        vector<Point_2> tempPolygonPointSet = insertPointToPolygonPointSet(currentPoint,tempEdgeToReturn,polygonPointSet);
        double tempMinArea = getSimplePolygonFromPoints(currentPolygonPointSet).area();
        if (tempMinArea < minArea) {
            edgeToReturn = replaceableEdges[i];
            minArea = tempMinArea;
        }
    }
    return edgeToReturn;
}

Segment_2 maxAreaEdgeSelection(vector<Segment_2> replaceableEdges, Point_2 currentPoint, vector<Point_2> polygonPointSet) {
    Segment_2 edgeToReturn = replaceableEdges[0];
    vector<Point_2> currentPolygonPointSet = insertPointToPolygonPointSet(currentPoint,edgeToReturn,polygonPointSet);
    double maxArea = getSimplePolygonFromPoints(currentPolygonPointSet).area();
    for(int i=1 ; i<replaceableEdges.size() ; ++i) {
        Segment_2 tempEdgeToReturn = replaceableEdges[i];
        vector<Point_2> tempPolygonPointSet = insertPointToPolygonPointSet(currentPoint,tempEdgeToReturn,polygonPointSet);
        double tempMaxArea = getSimplePolygonFromPoints(currentPolygonPointSet).area();
        if (tempMaxArea > maxArea) {
            edgeToReturn = replaceableEdges[i];
            maxArea = tempMaxArea;
        }
    }
    return edgeToReturn;
}

Segment_2 getPolygonEdgeToReplace(vector<Segment_2> replaceableEdges, Point_2 currentPoint, vector<Point_2> polygonPointSet, int edgeSelectionMethod) {
    switch (edgeSelectionMethod) {
        case 1:
            return randomEdgeSelection(replaceableEdges);
        case 2:
            return minAreaEdgeSelection(replaceableEdges,currentPoint,polygonPointSet);
        case 3:
            return maxAreaEdgeSelection(replaceableEdges,currentPoint,polygonPointSet);
        default:
            exit(-1);
    }
}

vector<Point_2> insertPointToPolygonPointSet(Point_2 point, Segment_2 edgeToBreak, vector<Point_2> polygon) {
    auto indexOfEdge1 = find(polygon.begin(), polygon.end(), edgeToBreak[0]);
    auto indexOfEdge2 = find(polygon.begin(), polygon.end(), edgeToBreak[1]);
    int index1 = indexOfEdge1 - polygon.begin();
    int index2 = indexOfEdge2 - polygon.begin();
    int startingIndex;
    (index1 > index2) ? (startingIndex = index2) : (startingIndex = index1);
    auto insertPosition = polygon.begin() + startingIndex+1;
    polygon.insert(insertPosition,point);
    return polygon;
}

vector<Point_2> IncrementalAlg(vector<Point_2> pointSet, int edgeSelectionMethod, string initMethod) {
    vector<Point_2> sortedPointSet, usedPoints, convexHull, polygon;

    sortedPointSet = sortPointset(pointSet, initMethod);

    //cout<<"Starting triangle is:\n";
    for(int i=0; i<3; i++) {
        polygon.push_back(sortedPointSet.front());
        //cout<<"["<<sortedPointSet.front().x()<<" ]["<<sortedPointSet.front().y()<<"]"<<endl;
        usedPoints.push_back(sortedPointSet.front());
        sortedPointSet.erase(sortedPointSet.begin());
    }
    //cout<<endl;

    while(!sortedPointSet.empty()){
        //cout<<"Creating CH!"<<endl;
        convexHull = createConvexHull(usedPoints);

        Point_2 currentPoint = sortedPointSet.front();
        sortedPointSet.erase(sortedPointSet.begin());
        usedPoints.push_back(currentPoint);

        //cout<<"Creating the new CH!"<<endl;
        vector<Point_2> newConvexHull = createConvexHull(usedPoints);
        //cout<<"Calculating Red Edges when adding Point ["<<currentPoint.x()<<" , "<<currentPoint.y()<<"]"<<endl;
        vector<Segment_2> redEdges = getRedEdges(convexHull, newConvexHull,currentPoint);
        //cout<<"Calculating Replaceable Edges!"<<endl;
        vector<Segment_2> replaceableEdges = calculateVisibleEdges(redEdges,polygon,currentPoint,convexHull);
        //cout<<"Calculating Edge to Replace!"<<endl;
        Segment_2 edgeToReplace = getPolygonEdgeToReplace(replaceableEdges,currentPoint,polygon,edgeSelectionMethod);
        //cout<<"Replacing Edge ["<<edgeToReplace.start().x()<<" ,"<<edgeToReplace.start().y()<<"]["<<edgeToReplace.end().x()<<" ,"<<edgeToReplace.end().y()<<"]"<<endl;
        polygon = insertPointToPolygonPointSet(currentPoint,edgeToReplace,polygon);
        //cout<<endl<<endl;
    }

    return polygon;

    //printPointSet(polygon);
    //Polygon_2 polygon2 = getSimplePolygonFromPoints(polygon);
    //cout << "Polygon is " << (polygon2.is_simple() ? "" : "not") << " simple." << endl;
}

/*--------------------------------------------------------------------------------------------------------------------*/

void printResults(vector<Point_2> points, int edgeSelection, string algorithm, string incrementalInit, unsigned long milliseconds){
    cout<<"Polygonization"<<endl;
    for(Point_2 point : points)
        cout<<point.x()<<" "<<point.y()<<endl;

    for(int i=0; i<(points.size()-1); i++)
        cout<<points[i].x()<<" "<<points[i].y()<<" "<<points[i+1].x()<<" "<<points[i+1].y()<<endl;

    int size = points.size();
    cout<<points[size-1].x()<<" "<<points[size-1].y()<<" "<<points[0].x()<<" "<<points[0].y()<<endl;

    if(algorithm=="incremental")
        cout<<"Algorithm: incremental"<<endl;
    else if(algorithm=="convex_hull")
        cout<<"Algorithm: convex_hull"<<endl;

    Polygon_2 polygon = getSimplePolygonFromPoints(points);
    double polygonArea = abs(polygon.area());
    cout<<"area: "<<polygonArea<<endl;
    cout<<"ratio: "<<(polygonArea/abs(getSimplePolygonFromPoints(createConvexHull(points)).area()))*100<<"%"<<endl;
    cout<<"construcution time:"<<milliseconds<<"ms"<<endl;
}

int main(int argc, char* argv[]) {
    string inputFileName;
    string outputFileName;
    string algorithmName;
    string edgeSelection;
    string incrementalInit;
    for(int i=1 ; i < argc ; i+=2) {
        if (strcmp(argv[i],"-i")==0) {
            inputFileName = argv[i+1];
        }
        else if (strcmp(argv[i],"-o")==0) {
            outputFileName = argv[i+1];
        }
        else if (strcmp(argv[i],"-algorithm")==0) {
            algorithmName = argv[i+1];
        }
        else if (strcmp(argv[i],"-edge_selection")==0) {
            edgeSelection = argv[i+1];
        }
        else if (strcmp(argv[i],"-initialization")==0) {
            incrementalInit = argv[i+1];
        }
    }

    //printUserInput(inputFileName,outputFileName,algorithmName,edgeSelection,incrementalInit);
    vector<Point_2> test = ProcessInputFile(inputFileName);

    auto started = std::chrono::high_resolution_clock::now();
    vector<Point_2> result = IncrementalAlg(test, stoi(edgeSelection), incrementalInit);
    auto done = std::chrono::high_resolution_clock::now();
    
    printResults(result, stoi(edgeSelection), algorithmName, incrementalInit, std::chrono::duration_cast<std::chrono::milliseconds>(done-started).count());

    return 0;
}

// ./main -i test.txt -o outputFile.txt -algorithm incremental -edge_selection 1 -initialization 2b

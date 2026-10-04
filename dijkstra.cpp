#include <iostream>
#include <vector>
#include <climits>
#include <algorithm>
#include <cmath>

#include <QApplication>
#include <QWidget>
#include <QSlider>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QInputDialog>
#include <QOpenGLWidget>
#include <QPainter>
#include <GL/gl.h>
#include <QTimer>

#include <QMouseEvent>
#include <QKeyEvent>





using namespace std;

struct node{
    int id;
    float x,y;
    bool active;
};

struct edge{
    int from, to , weight;
    bool directed;
    bool customWeight;
};



class Graph{
    private:
    vector<node> nodes;
    vector<edge> edges;

    public:
    void addNode(float x , float y){
        int id = nodes.size();
        nodes.push_back({
            id, x , y , true
        });
    }

    void moveNode(int id, float x, float y){
        if(id < 0 || id >= nodes.size()){
            cout << "Invalid node ID\n";
            return;
        }

        if(!nodes[id].active){
            cout << "Node already removed\n";
            return;
        }

        nodes[id].x = x;
        nodes[id].y = y;
    }

    void addEdge(int from, int to, int weight, bool directed){
        if(from < 0 || from >= nodes.size() || to < 0 || to >= nodes.size()){
            cout << "Invalid node ID\n";
            return;
        }

        if(weight < 0){
            cout << "Dijkstra cannot use negative edge weights\n";
            return;
        }

        if(edgeExists(from, to, directed)){
            cout << "Edge already exists\n";
            return;
        }

        edges.push_back({
            from, to, weight, directed , false
        });
    }

    void removeEdge(int from, int to){
        if(from < 0 || from >= nodes.size() || to < 0 || to >= nodes.size()){
            cout << "Invalid node ID\n";
            return;
        }

        for(auto it = edges.begin(); it != edges.end(); ++it){
            if((it->directed && it->from == from && it->to == to) || (!it->directed && ((it->from == from && it->to == to) || (it->from == to && it->to == from)))){
                edges.erase(it);
                return;
            }
        }

        cout << "Edge not found\n";
    }

    void changeEdgeWeight(int from, int to, int newWeight){
        if(from < 0 || from >= nodes.size() || to < 0 || to >= nodes.size()){
            cout << "Invalid node ID\n";
            return;
        }

        if(newWeight < 0){
            cout << "Dijkstra cannot use negative edge weights\n";
            return;
        }

        for(auto& e : edges){
            if((e.from == from && e.to == to) || (!e.directed && e.from == to && e.to == from)){
                e.weight = newWeight;
                e.customWeight = true;
                return;
            }
        }

        cout << "Edge not found\n"; 
    }

    void removeNode(int id){
        if(id < 0 || id >= nodes.size()){
            cout << "Invalid node ID\n";
            return;
        }

        if(!nodes[id].active){
            cout << "Node already removed\n";
            return;
        }

        nodes[id].active = false;

        for(auto it = edges.begin(); it != edges.end(); ){
            if(it->from == id || it->to == id){
                it = edges.erase(it);
            }
            else{
                ++it;
            }
        }
    }

    void updateAutomaticWeights(float scale){
        if(scale <= 0){
            return;
        }

        for(auto& e : edges){
            if(e.customWeight){
                continue;
            }

            float dx = nodes[e.from].x - nodes[e.to].x;
            float dy = nodes[e.from].y - nodes[e.to].y;

            float distance = sqrt(dx * dx + dy * dy);

            e.weight = max(1, static_cast<int>(round(distance / scale)));
        }
    }

    bool edgeExists(int from, int to, bool directed){
        for(const edge& e : edges){
            if(directed){
                if(e.directed && e.from == from && e.to == to){
                    return true;
                }
            }
            else{
                if(!e.directed && ((e.from == from && e.to == to) || (e.from == to && e.to == from))){
                    return true;
                }
            }
        }
        return false;
    }

    

    vector<vector<pair<int, int>>> getAdjacencyList() const{
        vector<vector<pair<int, int>>> adjacency(nodes.size());

        for(const edge& e : edges){
            if(!nodes[e.from].active || !nodes[e.to].active){
                continue;
            }

            adjacency[e.from].push_back({e.to, e.weight});

            if(!e.directed){
                adjacency[e.to].push_back({e.from, e.weight});
            }
        }
    return adjacency;
    }
    
    const  vector<node>& getNodes() const{
        return nodes;
    }
    
    bool isNodeActive(int id) const{
        if(id < 0 || id >= nodes.size()){
            return false;
        }
        return nodes[id].active;
    }

    const vector<edge>& getEdges() const{
        return edges;
    }
};


vector<int> getShortestPath(int source, int destination, const vector<int>& parent){
    vector<int> path;
    if(destination < 0 || destination >= parent.size()){
        return path;
    }
    int current = destination;

    while(current != -1){
        path.push_back(current);
        if(current == source){
            break;
        }
        current = parent[current];
    }

    if(path.empty() || path.back() != source){
        path.clear();
        return path;
    }

    reverse(path.begin(), path.end());
    return path;
}



void dijkstra(const Graph& graph, int source, vector<int>& dist, vector<int>& parent);

void drawCircle(float cx, float cy, float radius){
    glBegin(GL_LINE_LOOP);

    for(int i = 0; i < 100; i++){
        float angle = 2.0f * 3.14159f * i / 100;

        float x = cx + radius * cos(angle);
        float y = cy + radius * sin(angle);

        glVertex2f(x, y);
    }

    glEnd();
}

class GraphWidget : public QOpenGLWidget{
    private:
    Graph& graph;
    int selectedNode = -1;
    int sourceNode = -1;
    int destinationNode = -1;
    bool dragging = false;
    bool addNodeMode = false;
    bool addEdgeMode = false;
    bool directedMode = false;
    bool sourceMode = false;
    bool destinationMode = false;
    int edgeStartNode = -1;
    int selectedEdge = -1;
    float weightScale = 50.0f;
    vector<int> dist;
    vector<int> parent;
    vector<int> shortestPath;
    vector<bool> visited;
    int currentNode = -1;
    int currentEdgeFrom = -1;
    int currentEdgeTo = -1;
    int currentNeighborIndex = 0;
    vector<vector<pair<int, int>>> dijkstraAdjacency;
    bool dijkstraRunning = false;
    bool hasResult = false;
    QTimer *dijkstraTimer;
    QLabel *statusLabel;

    int findNodeAt(float x, float y){
        const vector<node>& nodes = graph.getNodes();

        for(const node& n : nodes){
            if(!n.active){
                continue;
            }

            float dx = x - n.x;
            float dy = y - n.y;

            if(dx * dx + dy * dy <= 15.0f * 15.0f){
                return n.id;
            }
        }
        return -1;
    }

    int findEdgeAt(float x, float y){
        const vector<node>& nodes = graph.getNodes();
        const vector<edge>& edges = graph.getEdges();

        for(int i = 0; i < edges.size(); i++){
            const edge& e = edges[i];

            if(!nodes[e.from].active || !nodes[e.to].active){
                continue;
            }

            float x1 = nodes[e.from].x;
            float y1 = nodes[e.from].y;

            float x2 = nodes[e.to].x;
            float y2 = nodes[e.to].y;

            float dx = x2 - x1;
            float dy = y2 - y1;

            float lengthSquared = dx * dx + dy * dy;

            if(lengthSquared == 0){
                continue;
            }

            float t = ((x - x1) * dx + (y - y1) * dy) / lengthSquared;

            if(t < 0){
                t = 0;
            }
            else if(t > 1){
                t = 1;
            }

            float closestX = x1 + t * dx;
            float closestY = y1 + t * dy;
            float distanceX = x - closestX;
            float distanceY = y - closestY;
            float distanceSquared = distanceX * distanceX + distanceY * distanceY;

            if(distanceSquared <= 8.0f * 8.0f){
                return i;
            }
        }
        return -1;
    }
    
    bool isPathEdge(const edge& e){
        for(int i = 0; i < shortestPath.size() - 1; i++){
            int from = shortestPath[i];
            int to = shortestPath[i + 1];
            if(e.directed){
                if(e.from == from && e.to == to){
                    return true;
                }
            }
            else{
                if((e.from == from && e.to == to) ||
                    (e.from == to && e.to == from)){
                    return true;
                }
            }
        }
        return false;
    }

    QPointF graphToScreen(float x, float y){
        float screenX = x * width() / 550.0f;
        float screenY = y * height() / 350.0f;
        return QPointF(screenX, screenY);
    }

    void drawGraph(const Graph& graph){
        const vector<node>& nodes = graph.getNodes();
        const vector<edge>& edges = graph.getEdges();

        for(int i = 0; i < edges.size(); i++){
            const edge& e = edges[i];
            if(!nodes[e.from].active || !nodes[e.to].active){
            continue;
            }
            if((e.from == currentEdgeFrom && e.to == currentEdgeTo) ||  (!e.directed && e.from == currentEdgeTo && e.to == currentEdgeFrom)){
                glColor3f(1.0f, 1.0f, 0.0f);
            }
            else if(i == selectedEdge){
                glColor3f(1.0f, 0.5f, 0.0f);
            }   
            else if(hasResult && isPathEdge(e)){
                glColor3f(0.0f, 1.0f, 0.0f);
            }
            else{
                glColor3f(1.0f, 1.0f, 1.0f);
            }

            float x1 = nodes[e.from].x;
            float y1 = nodes[e.from].y;

            float x2 = nodes[e.to].x;
            float y2 = nodes[e.to].y;

            glBegin(GL_LINES);

            glVertex2f(x1, y1);
            glVertex2f(x2, y2);

            glEnd();

            if(e.directed){
                float dx = x2 - x1;
                float dy = y2 - y1;
            
                float length = sqrt(dx * dx + dy * dy);
            
                if(length > 0){
                    dx /= length;
                    dy /= length;
                
                    float arrowSize = 10.0f;
                
                    float arrowX = x2 - dx * 15.0f;
                    float arrowY = y2 - dy * 15.0f;
                
                    float leftX = arrowX - dx * arrowSize + dy * arrowSize;
                    float leftY = arrowY - dy * arrowSize - dx * arrowSize;
                
                    float rightX = arrowX - dx * arrowSize - dy * arrowSize;
                    float rightY = arrowY - dy * arrowSize + dx * arrowSize;
                
                    glBegin(GL_LINES);
                
                    glVertex2f(arrowX, arrowY);
                    glVertex2f(leftX, leftY);
                
                    glVertex2f(arrowX, arrowY);
                    glVertex2f(rightX, rightY);
                
                    glEnd();
                }
            }
        }

        for(const node& n : nodes){
            if(!n.active){
                continue;
            }
            if(n.id == currentNode){
                glColor3f(1.0f, 1.0f, 0.0f);
            }
            else if(n.id == sourceNode){
                glColor3f(0.0f, 1.0f, 0.0f);
            }
            else if(n.id == destinationNode){
                glColor3f(1.0f, 0.0f, 0.0f);
            }
            else if(dijkstraRunning && visited[n.id]){
                glColor3f(0.2f, 0.6f, 1.0f);
            }
            else if(hasResult && dist[n.id] != INT_MAX){
                glColor3f(0.2f, 0.6f, 1.0f);
            }
            else{
                glColor3f(1.0f, 1.0f, 1.0f);
            }
            drawCircle(n.x, n.y, 15.0f);
        }

        if(selectedNode != -1){
            const node& n = nodes[selectedNode];
            glColor3f(1.0f, 0.5f, 0.0f);
            drawCircle(n.x, n.y, 20.0f);
        }

        if(edgeStartNode != -1){
            const node& n = nodes[edgeStartNode];
            glColor3f(0.0f, 1.0f, 0.0f);
            drawCircle(n.x, n.y, 20.0f);
        }
    }

    void startDijkstra(){
        if(sourceNode == -1 && destinationNode == -1){
            cout << "No source or destination node selected\n";
            if(statusLabel){
                statusLabel->setText(
                    "No source or destination selected. Press S and T to select them."
                );
            }
            return;
        }

        if(sourceNode == -1){
            cout << "No source node selected\n";
            if(statusLabel){
                statusLabel->setText(
                    "No source selected. Press S and click a node."
                );
            }
            return;
        }

        if(destinationNode == -1){
            cout << "No destination node selected\n";
            if(statusLabel){
                statusLabel->setText(
                    "No destination selected. Press T and click a node."
                );
            }
            return;
        }

        int n = graph.getNodes().size();
        dist.assign(n, INT_MAX);
        parent.assign(n, -1);
        visited.assign(n, false);
        shortestPath.clear();
        dist[sourceNode] = 0;
        currentNode = -1;
        currentEdgeFrom = -1;
        currentEdgeTo = -1;
        currentNeighborIndex = 0;
        dijkstraAdjacency = graph.getAdjacencyList();
        dijkstraRunning = true;
        hasResult = false;

        if(statusLabel){
            statusLabel->setText(
                QString("Running Dijkstra...  Source: %1  →  Destination: %2")
                    .arg(sourceNode)
                    .arg(destinationNode)
            );
        }
        cout << "Dijkstra animation started\n";
    }   

    void dijkstraStep(){
        if(!dijkstraRunning){
            return;
        }

        if(currentNode == -1){
            int u = -1;
            for(int i = 0; i < dijkstraAdjacency.size(); i++){
                if(!visited[i] &&
                    dist[i] != INT_MAX &&
                    (u == -1 || dist[i] < dist[u])){
                    u = i;
                }
            }

            if(u == -1){
                dijkstraRunning = false;
                currentNode = -1;
                currentEdgeFrom = -1;
                currentEdgeTo = -1;
                cout << "No path exists between source and destination\n";

                if(statusLabel){
                    statusLabel->setText(
                        QString("No path exists from Node %1 to Node %2")
                            .arg(sourceNode)
                            .arg(destinationNode)
                    );
                }
                update();
                return;
        }

            currentNode = u;
            currentNeighborIndex = 0;
            visited[u] = true;
            cout << "Visiting node: " << u << "\n";
        }

        if(currentNeighborIndex < dijkstraAdjacency[currentNode].size()){
            int v = dijkstraAdjacency[currentNode][currentNeighborIndex].first;
            int weight = dijkstraAdjacency[currentNode][currentNeighborIndex].second;
            currentEdgeFrom = currentNode;
            currentEdgeTo = v;
            cout << "Checking edge "
                << currentNode << " -> " << v << "\n";

            if(dist[currentNode] + weight < dist[v]){
                dist[v] = dist[currentNode] + weight;
                parent[v] = currentNode;
                cout << "Updated node " << v << " distance to " << dist[v] << "\n";
            }

            currentNeighborIndex++;
            update();
            return;
        }

        if(currentNode == destinationNode){
            dijkstraRunning = false;
            currentEdgeFrom = -1;
            currentEdgeTo = -1;
            shortestPath = getShortestPath(
                sourceNode,
                destinationNode,
                parent
            );
            hasResult = true;

            cout << "Destination reached\n";
            cout << "Shortest distance: " << dist[destinationNode] << "\n";     
            if(statusLabel){
                statusLabel->setText(
                    QString("Shortest path found!  Distance: %1  |  %2 → %3")
                        .arg(dist[destinationNode])
                        .arg(sourceNode)
                        .arg(destinationNode)
                );
            }
            currentNode = -1;
            return;
        }
        currentNode = -1;
        currentEdgeFrom = -1;
        currentEdgeTo = -1;
        update();
    }
    
    void clearDijkstraResult(){
        dijkstraRunning = false;
        hasResult = false;
        shortestPath.clear();
        dist.clear();
        parent.clear();
        visited.clear();
        currentNode = -1;
        currentEdgeFrom = -1;
        currentEdgeTo = -1;

        if(dijkstraTimer){
            dijkstraTimer->stop();
        }
        update();
    }

    public:
    GraphWidget(Graph& graph) : graph(graph){
        setFocusPolicy(Qt::StrongFocus);
        dijkstraTimer = new QTimer(this);
        statusLabel = nullptr;
        sourceNode = 0;
        destinationNode = 12;

        connect(dijkstraTimer, &QTimer::timeout, this, [this](){
            dijkstraStep();
            update();
            if(!dijkstraRunning){
                dijkstraTimer->stop();
            }
        });
    }

    void setStatusLabel(QLabel *label){
        statusLabel = label;
        updateStatusBar();
    }

    void updateStatusBar(){
        if(statusLabel == nullptr){
            return;
        }

        QString mode = "NORMAL";
        if(addNodeMode){
            mode = "ADD NODE";
        }
        else if(addEdgeMode){
            mode = "ADD EDGE";
        }
        else if(sourceMode){
            mode = "SELECT SOURCE";
        }
        else if(destinationMode){
            mode = "SELECT DESTINATION";
        }

        QString sourceText;
        QString destinationText;

        if(sourceNode == -1){
            sourceText = "None";
        }
        else{
            sourceText = QString::number(sourceNode);
        }

        if(destinationNode == -1){
            destinationText = "None";
        }
        else{
            destinationText = QString::number(destinationNode);
        }

        QString status =
            "MODE: " + mode +
            "    |    SOURCE: " + sourceText +
            "    |    DESTINATION: " + destinationText +
            "    |    R: Run Dijkstra" +
            "    |    S: Source" +
            "    |    T: Destination" +
            "    |    N: Node" +
            "    |    E: Edge" +
            "    |    D: Directed" +
            "    |    W: Weight" +
            "    |    Delete: Remove";

        statusLabel->setText(status);
    }

    void setWeightScale(float scale){
        weightScale = scale;
        graph.updateAutomaticWeights(weightScale);
        update();
    }

    protected:
    void initializeGL() override{
    }

    void paintGL() override{
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        drawGraph(graph);
        QPainter painter(this);
        painter.setPen(Qt::white);    
        const vector<node>& nodes = graph.getNodes();
        const vector<edge>& edges = graph.getEdges();

        for(const edge& e : edges){
            if(!nodes[e.from].active || !nodes[e.to].active){
                continue;
            }
            float x = (nodes[e.from].x + nodes[e.to].x) / 2.0f;
            float y = (nodes[e.from].y + nodes[e.to].y) / 2.0f;
            QPointF position = graphToScreen(x, y - 10);
            painter.drawText(position, QString::number(e.weight));
        }

        if(dijkstraRunning || hasResult){
            for(const node& n : graph.getNodes()){
                if(!n.active){
                    continue;
                }

                QString distanceText;
    
                if(dist.size() > n.id && dist[n.id] != INT_MAX){
                    distanceText = QString::number(dist[n.id]);
                }
                else{
                    distanceText = "∞";
                }
            
                QPointF position = graphToScreen(n.x + 18, n.y - 18);
                painter.drawText(position, distanceText);
            }
        }
    }

    void resizeGL(int width, int height) override{
        glViewport(0, 0, width, height);

        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();

        glOrtho(0, 550, 350, 0, -1, 1);

        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
    }
    
    void mousePressEvent(QMouseEvent *event) override{
        float x = static_cast<float>(event->x()) * 550.0f / width();
        float y = static_cast<float>(event->y()) * 350.0f / height();

        if(sourceMode){
            int clickedNode = findNodeAt(x, y);
            if(clickedNode != -1){
                sourceNode = clickedNode;
                sourceMode = false;
                updateStatusBar();
                cout << "Source node selected: " << sourceNode << '\n';
                update();
            }
            return;
        }   

        if(destinationMode){
            int clickedNode = findNodeAt(x, y); 
            if(clickedNode != -1){
                destinationNode = clickedNode;
                destinationMode = false;
                updateStatusBar();
                cout << "Destination node selected: " << destinationNode << '\n';
                update();
            }
            return;
        }

        if(addNodeMode){
            if(findNodeAt(x, y) == -1){
                clearDijkstraResult();
                graph.addNode(x, y);
                cout << "Node added\n";
                update();
            }
            return;
        }

        if(addEdgeMode){
            int clickedNode = findNodeAt(x, y);

            if(clickedNode != -1){
                if(edgeStartNode == -1){
                    edgeStartNode = clickedNode;
                    cout << "Edge start node: " << edgeStartNode << '\n';
                }
                else{
                    cout << "Edge end node: " << clickedNode << '\n';
                    float dx = graph.getNodes()[edgeStartNode].x - graph.getNodes()[clickedNode].x;
                    float dy = graph.getNodes()[edgeStartNode].y - graph.getNodes()[clickedNode].y;
                    float distance = sqrt(dx * dx + dy * dy);           
                    int weight = max(1, static_cast<int>(round(distance / weightScale)));
                    clearDijkstraResult();
                    graph.addEdge(edgeStartNode, clickedNode, weight, directedMode);
                    cout << "Edge added\n";
                    edgeStartNode = -1;
                }
                update();
            }
            return;
        }

        selectedNode = findNodeAt(x, y);

        if(selectedNode != -1){
            selectedEdge = -1;
            dragging = true;
            cout << "Selected node: " << selectedNode << '\n';
        }
        else{
            dragging = false;
            selectedEdge = findEdgeAt(x, y);
            if(selectedEdge != -1){
                cout << "Selected edge: " << selectedEdge << '\n';
            }
            else{
                cout << "Clicked empty space\n";
            }
        }
        update();
    }   

    void mouseMoveEvent(QMouseEvent *event) override{
        if(!dragging || selectedNode == -1){
            return;
        }
        float x = static_cast<float>(event->x()) * 550.0f / width();
        float y = static_cast<float>(event->y()) * 350.0f / height();

        graph.moveNode(selectedNode, x, y);
        graph.updateAutomaticWeights(weightScale);
        update();
    }

    void mouseReleaseEvent(QMouseEvent *event) override{
        if(event->button() == Qt::LeftButton){
            dragging = false;
        }
    }

    void keyPressEvent(QKeyEvent *event) override{
        if(event->key() == Qt::Key_N){
            addNodeMode = !addNodeMode;
            addEdgeMode = false;
            sourceMode = false;
            destinationMode = false;
            selectedEdge = -1;
            selectedNode = -1;
            edgeStartNode = -1;
            updateStatusBar();

            if(addNodeMode){
                cout << "Add node mode: ON\n";
            }
            else{
                cout << "Add node mode: OFF\n";
            }
        }
        else if(event->key() == Qt::Key_E){
            addEdgeMode = !addEdgeMode;
            addNodeMode = false;
            sourceMode = false;
            destinationMode = false;
            selectedEdge = -1;
            selectedNode = -1;
            edgeStartNode = -1;
            updateStatusBar();

            if(addEdgeMode){
                cout << "Add edge mode: ON\n";
            }
            else{
                cout << "Add edge mode: OFF\n";
            }
        }
        else if(event->key() == Qt::Key_W){
            if(selectedEdge == -1){
                cout << "No edge selected\n";
                return;
            }
            bool ok;
            int currentWeight = graph.getEdges()[selectedEdge].weight;
        
            int newWeight = QInputDialog::getInt(
                this,
                "Change Edge Weight",
                "Enter new weight:",
                currentWeight,
                1,
                100000,
                1,
                &ok
            );

            if(ok){
                const edge& e = graph.getEdges()[selectedEdge];
                clearDijkstraResult();
                graph.changeEdgeWeight(e.from, e.to, newWeight);
                cout << "Edge weight changed to: " << newWeight << '\n';
                update();
            }
        }
        else if(event->key() == Qt::Key_D){
            directedMode = !directedMode;
            updateStatusBar();
            if(directedMode){
                cout << "Directed edge mode: ON\n";
            }
            else{
                cout << "Directed edge mode: OFF\n";
            }
        }
        else if(event->key() == Qt::Key_Delete){
            if(selectedNode != -1){
                clearDijkstraResult();
                if(selectedNode == sourceNode){
                    sourceNode = -1;
                }
                if(selectedNode == destinationNode){
                    destinationNode = -1;
                }
                graph.removeNode(selectedNode);
                selectedNode = -1;
                cout << "Node removed\n";
                update();
            }   
            else if(selectedEdge != -1){
                const edge& e = graph.getEdges()[selectedEdge];
                clearDijkstraResult();
                graph.removeEdge(e.from, e.to);
                selectedEdge = -1;
                cout << "Edge removed\n";
                update();
            }
            else{
                cout << "Nothing selected\n";
            }
        }

        else if(event->key() == Qt::Key_S){
            sourceMode = true;
            destinationMode = false;
            addNodeMode = false;
            addEdgeMode = false;
            edgeStartNode = -1;
            updateStatusBar();
            cout << "Source selection mode: ON\n";
        }

        else if(event->key() == Qt::Key_T){
            destinationMode = true;
            sourceMode = false;
            addNodeMode = false;
            addEdgeMode = false;
            edgeStartNode = -1;
            updateStatusBar();
            cout << "Destination selection mode: ON\n";
        }

        else if(event->key() == Qt::Key_R){
            if(dijkstraRunning){
                cout << "Dijkstra is already running\n";
                if(statusLabel){
                    statusLabel->setText("Dijkstra is already running...");
                }
                return;
            }
            startDijkstra();
            if(dijkstraRunning){
                dijkstraTimer->start(700);
            }
        }
    }    
};




int main(int argc, char *argv[]){
    QApplication app(argc, argv);

    Graph graph;
    graph.addNode(80, 80);
    graph.addNode(190, 60);
    graph.addNode(300, 90);
    graph.addNode(410, 65);
    graph.addNode(520, 90);
    graph.addNode(130, 175); 
    graph.addNode(250, 155);  
    graph.addNode(370, 180); 
    graph.addNode(490, 160); 
    
    graph.addNode(90, 280);   
    graph.addNode(230, 265);   
    graph.addNode(370, 290);   
    graph.addNode(510, 270);
    
    graph.addEdge(0, 1, 6, false);
    graph.addEdge(1, 2, 9, false);
    graph.addEdge(2, 3, 4, false);
    graph.addEdge(3, 4, 8, false);
    
    graph.addEdge(0, 5, 3, false);
    graph.addEdge(1, 5, 7, false);
    graph.addEdge(1, 6, 4, false);
    graph.addEdge(2, 6, 8, false);
    graph.addEdge(2, 7, 3, false);
    graph.addEdge(3, 7, 7, false);
    graph.addEdge(3, 8, 5, false);
    graph.addEdge(4, 8, 3, false);
    
    graph.addEdge(5, 6, 2, false);
    graph.addEdge(6, 7, 9, false);
    graph.addEdge(7, 8, 2, false);
    
    graph.addEdge(5, 9, 6, false);
    graph.addEdge(6, 10, 3, false);
    graph.addEdge(7, 11, 7, false);
    graph.addEdge(8, 12, 4, false);
    
    
    graph.addEdge(9, 10, 8, false);
    graph.addEdge(10, 11, 2, false);
    graph.addEdge(11, 12, 9, false);
    
    graph.addEdge(9, 11, 6, false);
    graph.addEdge(10, 12, 5, false);
    
    graph.addEdge(0, 6, 12, false);
    graph.addEdge(2, 5, 11, false);
    graph.addEdge(3, 7, 7, false);
    graph.addEdge(5, 10, 10, false);
    graph.addEdge(7, 12, 11, false);

    QWidget window;
    QVBoxLayout *layout = new QVBoxLayout(&window);
    GraphWidget *graphWidget = new GraphWidget(graph);
    
    QSlider *scaleSlider = new QSlider(Qt::Horizontal);
    QLabel *scaleLabel = new QLabel("Weight Scale: 50");

    scaleSlider->setMinimum(10);
    scaleSlider->setMaximum(200);
    scaleSlider->setValue(50);
    scaleSlider->setFixedHeight(30);

    QHBoxLayout *controls = new QHBoxLayout();
    controls->addWidget(scaleLabel);
    controls->addWidget(scaleSlider);
    layout->addWidget(graphWidget, 1);

    QLabel *statusLabel = new QLabel("MODE: NORMAL");
    statusLabel->setFixedHeight(28);
    statusLabel->setAlignment(Qt::AlignCenter);
    layout->addWidget(statusLabel);
    graphWidget->setStatusLabel(statusLabel);
    layout->addLayout(controls);

    QObject::connect(scaleSlider, &QSlider::valueChanged,[&](int value){graphWidget->setWeightScale(value);scaleLabel->setText("Weight Scale: " + QString::number(value));});
    window.resize(800, 650);
    window.setWindowTitle("Dijkstra Visualizer");
    window.show();
    return app.exec();
}

void dijkstra(const Graph& graph, int source, vector<int>& dist, vector<int>& parent){
    if(source < 0 || source >= graph.getNodes().size()){
        cout << "Invalid source node\n";
        return;
    }

    fill(dist.begin(), dist.end(), INT_MAX);
    fill(parent.begin(), parent.end(), -1);

    vector<vector<pair<int, int>>> adjacency = graph.getAdjacencyList();
    int V = adjacency.size();
    vector<bool> visited(V, false);
    dist[source] = 0;

    for(int count = 0; count < V ; count++){
        int u = -1;
        for(int i = 0; i < V; i++){
            if(!visited[i] && (u == -1 || dist[i] < dist[u])){
                u = i;
            }
        }
        if(u == -1 || dist[u] == INT_MAX){
            break;
        }
        visited[u] = true;
        for(auto edge: adjacency[u]){
            int v = edge.first;
            int weight = edge.second;
            
            if(dist[u] + weight < dist[v]){
                dist[v] = dist[u] + weight;
                parent[v] = u;
            }
        }
    }
}
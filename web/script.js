// =============================================================================
// script.js — Interactive Grid & Web UI Logic
// =============================================================================

const ROWS = 20;
const COLS = 40;

let grid = [];
let startNode = { row: 5, col: 5 };
let endNode = { row: 14, col: 34 };
let isMouseDown = false;
let isAnimating = false;

document.addEventListener('DOMContentLoaded', () => {
    initGrid();
    setupEventListeners();
});

function initGrid() {
    const container = document.getElementById('grid-container');
    container.style.gridTemplateColumns = `repeat(${COLS}, 22px)`;
    container.innerHTML = '';

    grid = [];
    for (let r = 0; r < ROWS; r++) {
        const row = [];
        for (let c = 0; c < COLS; c++) {
            const cell = document.createElement('div');
            cell.className = 'cell';
            cell.dataset.row = r;
            cell.dataset.col = c;

            if (r === startNode.row && c === startNode.col) {
                cell.classList.add('start');
            } else if (r === endNode.row && c === endNode.col) {
                cell.classList.add('end');
            }

            cell.addEventListener('mousedown', (e) => handleCellMouseDown(e, r, c));
            cell.addEventListener('mouseenter', () => handleCellMouseEnter(r, c));

            container.appendChild(cell);
            row.push({ isWall: false, weight: 1, element: cell });
        }
        grid.push(row);
    }

    document.addEventListener('mouseup', () => { isMouseDown = false; });
}

function handleCellMouseDown(e, r, c) {
    if (isAnimating) return;
    isMouseDown = true;

    if (e.shiftKey) {
        setStartNode(r, c);
    } else if (e.ctrlKey || e.metaKey) {
        setEndNode(r, c);
    } else {
        applyBrush(r, c);
    }
}

function handleCellMouseEnter(r, c) {
    if (isMouseDown && !isAnimating) {
        applyBrush(r, c);
    }
}

function applyBrush(r, c) {
    if ((r === startNode.row && c === startNode.col) || (r === endNode.row && c === endNode.col)) return;

    const brush = document.getElementById('brush-select').value;
    const cell = grid[r][c];

    cell.element.className = 'cell';

    if (brush === 'wall') {
        cell.isWall = true;
        cell.weight = 1;
        cell.element.classList.add('wall');
    } else if (brush === 'weight-2') {
        cell.isWall = false;
        cell.weight = 2;
        cell.element.classList.add('weight-2');
    } else if (brush === 'weight-5') {
        cell.isWall = false;
        cell.weight = 5;
        cell.element.classList.add('weight-5');
    } else if (brush === 'eraser') {
        cell.isWall = false;
        cell.weight = 1;
    }
}

function setStartNode(r, c) {
    grid[startNode.row][startNode.col].element.classList.remove('start');
    startNode = { row: r, col: c };
    grid[r][c].isWall = false;
    grid[r][c].element.className = 'cell start';
}

function setEndNode(r, c) {
    grid[endNode.row][endNode.col].element.classList.remove('end');
    endNode = { row: r, col: c };
    grid[r][c].isWall = false;
    grid[r][c].element.className = 'cell end';
}

function setupEventListeners() {
    document.getElementById('btn-run').addEventListener('click', runAlgorithm);
    document.getElementById('btn-compare').addEventListener('click', runCompareAll);
    document.getElementById('btn-clear-path').addEventListener('click', clearPath);
    document.getElementById('btn-clear-walls').addEventListener('click', clearBoard);
    document.getElementById('btn-maze-dfs').addEventListener('click', () => generateMaze('dfs'));
    document.getElementById('btn-maze-prims').addEventListener('click', () => generateMaze('prims'));
    document.getElementById('btn-close-modal').addEventListener('click', () => {
        document.getElementById('compare-modal').classList.add('hidden');
    });
}

function clearPath() {
    if (isAnimating) return;
    for (let r = 0; r < ROWS; r++) {
        for (let c = 0; c < COLS; c++) {
            const cell = grid[r][c];
            cell.element.classList.remove('visited', 'path');
        }
    }
}

function clearBoard() {
    if (isAnimating) return;
    for (let r = 0; r < ROWS; r++) {
        for (let c = 0; c < COLS; c++) {
            const cell = grid[r][c];
            cell.isWall = false;
            cell.weight = 1;
            cell.element.className = 'cell';
        }
    }
    grid[startNode.row][startNode.col].element.classList.add('start');
    grid[endNode.row][endNode.col].element.classList.add('end');
}

function getGridPayload() {
    const payloadGrid = [];
    for (let r = 0; r < ROWS; r++) {
        const row = [];
        for (let c = 0; c < COLS; c++) {
            row.push({
                isWall: grid[r][c].isWall,
                weight: grid[r][c].weight
            });
        }
        payloadGrid.push(row);
    }
    return payloadGrid;
}

async function runAlgorithm() {
    if (isAnimating) return;
    clearPath();

    const algo = document.getElementById('algorithm-select').value;
    const mode = document.getElementById('connectivity-select').value;

    const payload = {
        rows: ROWS,
        cols: COLS,
        mode: mode,
        start: startNode,
        end: endNode,
        algorithm: algo,
        grid: getGridPayload()
    };

    try {
        const response = await fetch('/api/solve', {
            method: 'POST',
            headers: { 'Content-Type': 'application/json' },
            body: JSON.stringify(payload)
        });

        const data = await response.json();
        if (data.error) {
            alert('Error: ' + data.error);
            return;
        }

        animateResult(data);
    } catch (err) {
        alert('Failed to connect to backend server: ' + err);
    }
}

function animateResult(data) {
    isAnimating = true;
    const visited = data.visitedOrder;
    const path = data.path;

    let delay = 0;
    visited.forEach((step, idx) => {
        setTimeout(() => {
            if ((step.row !== startNode.row || step.col !== startNode.col) &&
                (step.row !== endNode.row || step.col !== endNode.col)) {
                grid[step.row][step.col].element.classList.add('visited');
            }
            if (idx === visited.length - 1) {
                animatePath(path, data.stats);
            }
        }, delay);
        delay += 10;
    });

    if (visited.length === 0) {
        animatePath(path, data.stats);
    }
}

function animatePath(path, stats) {
    let delay = 0;
    path.forEach((step, idx) => {
        setTimeout(() => {
            if ((step.row !== startNode.row || step.col !== startNode.col) &&
                (step.row !== endNode.row || step.col !== endNode.col)) {
                grid[step.row][step.col].element.classList.add('path');
            }
            if (idx === path.length - 1) {
                isAnimating = false;
                updateStats(stats);
            }
        }, delay);
        delay += 30;
    });

    if (path.length === 0) {
        isAnimating = false;
        updateStats(stats);
    }
}

function updateStats(stats) {
    document.getElementById('stat-visited').innerText = stats.nodesVisited;
    document.getElementById('stat-length').innerText = stats.pathLength;
    document.getElementById('stat-cost').innerText = stats.pathCost.toFixed(1);
    document.getElementById('stat-time').innerText = stats.timeMs.toFixed(2);
}

async function generateMaze(algorithm) {
    if (isAnimating) return;
    clearBoard();

    try {
        const response = await fetch('/api/generate-maze', {
            method: 'POST',
            headers: { 'Content-Type': 'application/json' },
            body: JSON.stringify({ rows: ROWS, cols: COLS, algorithm: algorithm })
        });

        const data = await response.json();
        if (data.grid) {
            for (let r = 0; r < ROWS; r++) {
                for (let c = 0; c < COLS; c++) {
                    if (data.grid[r][c].isWall) {
                        grid[r][c].isWall = true;
                        grid[r][c].element.classList.add('wall');
                    }
                }
            }
        }
    } catch (err) {
        alert('Failed to generate maze: ' + err);
    }
}

async function runCompareAll() {
    if (isAnimating) return;

    const algos = ['dijkstra', 'astar-manhattan', 'astar-euclidean', 'bfs', 'bellman-ford'];
    const mode = document.getElementById('connectivity-select').value;
    const tableBody = document.getElementById('compare-table-body');
    tableBody.innerHTML = '<tr><td colspan="5">Running benchmark tests...</td></tr>';
    document.getElementById('compare-modal').classList.remove('hidden');

    const results = [];

    for (const algo of algos) {
        const payload = {
            rows: ROWS,
            cols: COLS,
            mode: mode,
            start: startNode,
            end: endNode,
            algorithm: algo,
            grid: getGridPayload()
        };

        try {
            const response = await fetch('/api/solve', {
                method: 'POST',
                headers: { 'Content-Type': 'application/json' },
                body: JSON.stringify(payload)
            });
            const data = await response.json();
            results.push(data);
        } catch (err) {
            console.error(err);
        }
    }

    tableBody.innerHTML = '';
    results.forEach(res => {
        const row = document.createElement('tr');
        row.innerHTML = `
            <td><strong>${res.algorithm}</strong></td>
            <td>${res.stats.nodesVisited}</td>
            <td>${res.stats.pathLength}</td>
            <td>${res.stats.pathCost.toFixed(1)}</td>
            <td>${res.stats.timeMs.toFixed(2)} ms</td>
        `;
        tableBody.appendChild(row);
    });
}

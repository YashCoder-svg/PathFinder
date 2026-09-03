// =============================================================================
// script.js — Interactive Grid & Web UI Logic (PathFinder Pro)
// =============================================================================

const ROWS = 20;
const COLS = 40;

let grid = [];
let startNode = { row: 5, col: 5 };
let endNode = { row: 14, col: 34 };
let activeMode = 'wall'; // 'start' | 'end' | 'wall'
let isMouseDown = false;
let isAnimating = false;
let isDraggingStart = false;
let isDraggingEnd = false;
let activeTimeouts = [];

document.addEventListener('DOMContentLoaded', () => {
    initGrid();
    setupEventListeners();
    setupKeyboardShortcuts();
    updateStatusBar();
    validateRunState();

    document.addEventListener('mouseup', () => {
        isMouseDown = false;
        isDraggingStart = false;
        isDraggingEnd = false;
    });
});

function initGrid() {
    const container = document.getElementById('grid-container');
    container.style.gridTemplateColumns = `repeat(${COLS}, 22px)`;
    container.innerHTML = '';

    // Prevent context menu on grid for right-click erase shortcut
    container.addEventListener('contextmenu', (e) => e.preventDefault());

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
}

function handleCellMouseDown(e, r, c) {
    if (isAnimating) return;
    e.preventDefault(); // Prevent browser drag & drop / text selection glitch
    isMouseDown = true;

    // Right-click shortcut: Erase cell
    if (e.button === 2) {
        eraseCell(r, c);
        return;
    }

    if (r === startNode.row && c === startNode.col) {
        isDraggingStart = true;
        setPlacementMode('start');
    } else if (r === endNode.row && c === endNode.col) {
        isDraggingEnd = true;
        setPlacementMode('end');
    } else if (activeMode === 'start' || e.shiftKey) {
        setStartNode(r, c);
    } else if (activeMode === 'end' || e.ctrlKey || e.metaKey) {
        setEndNode(r, c);
    } else {
        applyBrush(r, c);
    }
}

function handleCellMouseEnter(r, c) {
    if (isMouseDown && !isAnimating) {
        if (isDraggingStart) {
            setStartNode(r, c);
        } else if (isDraggingEnd) {
            setEndNode(r, c);
        } else if (activeMode === 'start') {
            setStartNode(r, c);
        } else if (activeMode === 'end') {
            setEndNode(r, c);
        } else {
            applyBrush(r, c);
        }
    }
}

function setPlacementMode(mode) {
    if (['start', 'end', 'wall'].includes(mode)) {
        activeMode = mode;
        document.querySelectorAll('.mode-btn').forEach(btn => {
            if (btn.dataset.mode === mode) {
                btn.classList.add('active');
            } else {
                btn.classList.remove('active');
            }
        });
        updateStatusBar();
    }
}

function eraseCell(r, c) {
    if ((r === startNode.row && c === startNode.col) || (r === endNode.row && c === endNode.col)) return;
    const cell = grid[r][c];
    cell.isWall = false;
    cell.weight = 1;
    cell.element.className = 'cell';
}

function applyBrush(r, c) {
    if ((r === startNode.row && c === startNode.col) || (r === endNode.row && c === endNode.col)) return;

    const brushSelect = document.getElementById('brush-select').value;
    const cell = grid[r][c];

    cell.element.className = 'cell';

    if (brushSelect === 'wall' || activeMode === 'wall') {
        cell.isWall = true;
        cell.weight = 1;
        cell.element.classList.add('wall');
    } else if (brushSelect === 'weight-2') {
        cell.isWall = false;
        cell.weight = 2;
        cell.element.classList.add('weight-2');
    } else if (brushSelect === 'weight-5') {
        cell.isWall = false;
        cell.weight = 5;
        cell.element.classList.add('weight-5');
    } else if (brushSelect === 'eraser') {
        cell.isWall = false;
        cell.weight = 1;
    }
}

function setStartNode(r, c) {
    if (r < 0 || r >= ROWS || c < 0 || c >= COLS) return;
    if (r === endNode.row && c === endNode.col) return;

    if (startNode && grid[startNode.row] && grid[startNode.row][startNode.col]) {
        grid[startNode.row][startNode.col].element.classList.remove('start');
    }

    startNode = { row: r, col: c };
    grid[r][c].isWall = false;
    grid[r][c].weight = 1;
    grid[r][c].element.className = 'cell start';

    document.getElementById('start-row').value = r;
    document.getElementById('start-col').value = c;

    clearInlineErrors();
    updateStatusBar();
    validateRunState();
}

function setEndNode(r, c) {
    if (r < 0 || r >= ROWS || c < 0 || c >= COLS) return;
    if (r === startNode.row && c === startNode.col) return;

    if (endNode && grid[endNode.row] && grid[endNode.row][endNode.col]) {
        grid[endNode.row][endNode.col].element.classList.remove('end');
    }

    endNode = { row: r, col: c };
    grid[r][c].isWall = false;
    grid[r][c].weight = 1;
    grid[r][c].element.className = 'cell end';

    document.getElementById('end-row').value = r;
    document.getElementById('end-col').value = c;

    clearInlineErrors();
    updateStatusBar();
    validateRunState();
}

function setupEventListeners() {
    document.getElementById('btn-run').addEventListener('click', runAlgorithm);
    document.getElementById('btn-compare').addEventListener('click', runCompareAll);
    document.getElementById('btn-clear-path').addEventListener('click', clearPath);
    document.getElementById('btn-clear-walls').addEventListener('click', clearBoard);
    document.getElementById('btn-maze-dfs').addEventListener('click', () => generateMaze('dfs'));
    document.getElementById('btn-maze-prims').addEventListener('click', () => generateMaze('prims'));
    document.getElementById('btn-randomize').addEventListener('click', randomizeStartEnd);
    document.getElementById('btn-apply-coords').addEventListener('click', applyManualCoordinates);

    document.getElementById('btn-dismiss-banner').addEventListener('click', hideNoPathBanner);

    const compareModal = document.getElementById('compare-modal');
    document.getElementById('btn-close-modal').addEventListener('click', () => {
        compareModal.classList.add('hidden');
    });

    // Backdrop click-to-close
    compareModal.addEventListener('click', (e) => {
        if (e.target === compareModal) {
            compareModal.classList.add('hidden');
        }
    });

    ['start-row', 'start-col', 'end-row', 'end-col'].forEach(id => {
        const el = document.getElementById(id);
        if (el) {
            el.addEventListener('keydown', (e) => {
                if (e.key === 'Enter') {
                    e.preventDefault();
                    applyManualCoordinates();
                }
            });
        }
    });

    // Placement mode toggle buttons
    document.querySelectorAll('.mode-btn').forEach(btn => {
        btn.addEventListener('click', (e) => {
            const mode = e.currentTarget.dataset.mode;
            setPlacementMode(mode);
        });
    });
}

function setupKeyboardShortcuts() {
    window.addEventListener('keydown', (e) => {
        const activeTag = document.activeElement ? document.activeElement.tagName.toUpperCase() : '';
        if (activeTag === 'INPUT' || activeTag === 'SELECT' || activeTag === 'TEXTAREA') {
            return;
        }

        if (e.key === 's' || e.key === 'S') {
            setPlacementMode('start');
        } else if (e.key === 'e' || e.key === 'E') {
            setPlacementMode('end');
        } else if (e.key === 'w' || e.key === 'W') {
            setPlacementMode('wall');
        } else if (e.key === 'Escape') {
            clearPath();
            hideNoPathBanner();
            document.getElementById('compare-modal').classList.add('hidden');
        }
    });
}

function applyManualCoordinates() {
    clearInlineErrors();

    const startRowVal = parseInt(document.getElementById('start-row').value, 10);
    const startColVal = parseInt(document.getElementById('start-col').value, 10);
    const endRowVal = parseInt(document.getElementById('end-row').value, 10);
    const endColVal = parseInt(document.getElementById('end-col').value, 10);

    let hasError = false;

    if (isNaN(startRowVal) || startRowVal < 0 || startRowVal >= ROWS ||
        isNaN(startColVal) || startColVal < 0 || startColVal >= COLS) {
        showInlineError('start-coord-error', `Start bounds: Row (0–${ROWS - 1}), Col (0–${COLS - 1})`);
        hasError = true;
    }

    if (isNaN(endRowVal) || endRowVal < 0 || endRowVal >= ROWS ||
        isNaN(endColVal) || endColVal < 0 || endColVal >= COLS) {
        showInlineError('end-coord-error', `End bounds: Row (0–${ROWS - 1}), Col (0–${COLS - 1})`);
        hasError = true;
    }

    if (!hasError && startRowVal === endRowVal && startColVal === endColVal) {
        showInlineError('start-coord-error', 'Start and End coordinates cannot be identical');
        showInlineError('end-coord-error', 'Start and End coordinates cannot be identical');
        hasError = true;
    }

    if (!hasError) {
        setStartNode(startRowVal, startColVal);
        setEndNode(endRowVal, endColVal);
    }
}

function showInlineError(elementId, message) {
    const errorEl = document.getElementById(elementId);
    if (errorEl) {
        errorEl.innerText = message;
        errorEl.classList.remove('hidden');
    }
}

function clearInlineErrors() {
    const startErr = document.getElementById('start-coord-error');
    const endErr = document.getElementById('end-coord-error');
    if (startErr) startErr.classList.add('hidden');
    if (endErr) endErr.classList.add('hidden');
}

function randomizeStartEnd() {
    if (isAnimating) return;

    let sr, sc, er, ec;
    let attempts = 0;
    const minDistance = 6;

    while (attempts < 200) {
        sr = Math.floor(Math.random() * ROWS);
        sc = Math.floor(Math.random() * COLS);
        er = Math.floor(Math.random() * ROWS);
        ec = Math.floor(Math.random() * COLS);

        const dist = Math.abs(sr - er) + Math.abs(sc - ec);
        if (dist >= minDistance && !grid[sr][sc].isWall && !grid[er][ec].isWall) {
            break;
        }
        attempts++;
    }

    setStartNode(sr, sc);
    setEndNode(er, ec);
    hideNoPathBanner();
}

function updateStatusBar() {
    const startText = startNode ? `(${startNode.row}, ${startNode.col})` : 'Not Set';
    const endText = endNode ? `(${endNode.row}, ${endNode.col})` : 'Not Set';
    const modeMap = { 'start': 'Place Start (🚩)', 'end': 'Place End (🎯)', 'wall': 'Place Walls (🧱)' };

    document.getElementById('status-start-coords').innerText = startText;
    document.getElementById('status-end-coords').innerText = endText;
    document.getElementById('status-active-mode').innerText = modeMap[activeMode] || activeMode;
}

function validateRunState() {
    const btnRun = document.getElementById('btn-run');
    if (!startNode || !endNode || (startNode.row === endNode.row && startNode.col === endNode.col)) {
        btnRun.disabled = true;
        btnRun.title = "Set valid start and end positions to run search";
    } else {
        btnRun.disabled = false;
        btnRun.title = "Run pathfinding algorithm";
    }
}

function showNoPathBanner() {
    const banner = document.getElementById('no-path-banner');
    if (banner) banner.classList.remove('hidden');
}

function hideNoPathBanner() {
    const banner = document.getElementById('no-path-banner');
    if (banner) banner.classList.add('hidden');
}

function clearAnimationTimeouts() {
    for (const t of activeTimeouts) {
        clearTimeout(t);
    }
    activeTimeouts = [];
    isAnimating = false;
}

function clearPath() {
    clearAnimationTimeouts();
    hideNoPathBanner();
    for (let r = 0; r < ROWS; r++) {
        for (let c = 0; c < COLS; c++) {
            const cell = grid[r][c];
            cell.element.classList.remove('visited', 'path');
        }
    }
}

function clearBoard() {
    clearAnimationTimeouts();
    hideNoPathBanner();
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

function getWallsArray() {
    const walls = [];
    for (let r = 0; r < ROWS; r++) {
        for (let c = 0; c < COLS; c++) {
            if (grid[r][c].isWall) {
                walls.push([r, c]);
            }
        }
    }
    return walls;
}

function getWeightsArray() {
    const weights = [];
    for (let r = 0; r < ROWS; r++) {
        for (let c = 0; c < COLS; c++) {
            if (!grid[r][c].isWall && grid[r][c].weight > 1) {
                weights.push({ row: r, col: c, weight: grid[r][c].weight });
            }
        }
    }
    return weights;
}

async function runAlgorithm() {
    if (isAnimating) return;
    clearPath();

    const algo = document.getElementById('algorithm-select').value;
    const mode = document.getElementById('connectivity-select').value;

    const payload = {
        grid: {
            width: COLS,
            height: ROWS,
            walls: getWallsArray(),
            weights: getWeightsArray()
        },
        mode: mode,
        start: [startNode.row, startNode.col],
        end: [endNode.row, endNode.col],
        algorithm: algo
    };

    try {
        const response = await fetch('/api/solve', {
            method: 'POST',
            headers: { 'Content-Type': 'application/json' },
            body: JSON.stringify(payload)
        });

        const data = await response.json();
        if (!response.ok || data.error) {
            alert('Validation Error: ' + (data.error || 'Failed request'));
            return;
        }

        if (data.noPathFound) {
            showNoPathBanner();
            updateStats(data.stats || { nodesVisited: 0, pathLength: 0, timeMs: 0 });
            return;
        }

        animateResult(data);
    } catch (err) {
        alert('Failed to connect to backend server: ' + err);
    }
}

function animateResult(data) {
    clearAnimationTimeouts();
    isAnimating = true;
    const visited = data.visitedOrder || [];
    const path = data.path || [];

    let delay = 0;
    visited.forEach((step, idx) => {
        const t = setTimeout(() => {
            const r = Array.isArray(step) ? step[0] : step.row;
            const c = Array.isArray(step) ? step[1] : step.col;
            if ((r !== startNode.row || c !== startNode.col) &&
                (r !== endNode.row || c !== endNode.col)) {
                grid[r][c].element.classList.add('visited');
            }
            if (idx === visited.length - 1) {
                animatePath(path, data.stats);
            }
        }, delay);
        activeTimeouts.push(t);
        delay += 10;
    });

    if (visited.length === 0) {
        animatePath(path, data.stats);
    }
}

function animatePath(path, stats) {
    let delay = 0;
    path.forEach((step, idx) => {
        const t = setTimeout(() => {
            const r = Array.isArray(step) ? step[0] : step.row;
            const c = Array.isArray(step) ? step[1] : step.col;
            if ((r !== startNode.row || c !== startNode.col) &&
                (r !== endNode.row || c !== endNode.col)) {
                grid[r][c].element.classList.add('path');
            }
            if (idx === path.length - 1) {
                isAnimating = false;
                updateStats(stats);
            }
        }, delay);
        activeTimeouts.push(t);
        delay += 30;
    });

    if (path.length === 0) {
        isAnimating = false;
        updateStats(stats);
    }
}

function updateStats(stats) {
    if (!stats) return;
    document.getElementById('stat-visited').innerText = stats.nodesVisited !== undefined ? stats.nodesVisited : 0;
    document.getElementById('stat-length').innerText = stats.pathLength !== undefined ? stats.pathLength : 0;
    document.getElementById('stat-cost').innerText = stats.pathCost !== undefined ? stats.pathCost.toFixed(1) : (stats.pathLength || 0);
    document.getElementById('stat-time').innerText = stats.executionTimeMs !== undefined ? stats.executionTimeMs.toFixed(2) : (stats.timeMs ? stats.timeMs.toFixed(2) : '0.00');
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
                    if ((r === startNode.row && c === startNode.col) || (r === endNode.row && c === endNode.col)) {
                        grid[r][c].isWall = false;
                    } else if (data.grid[r][c].isWall) {
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

    const algos = [
        'astar-manhattan',
        'astar-euclidean',
        'dijkstra',
        'bidirectional-astar',
        'bidirectional-dijkstra',
        'bidirectional-bfs',
        'greedy-best-first',
        'bfs',
        'dfs',
        'iddfs',
        'bellman-ford'
    ];
    const mode = document.getElementById('connectivity-select').value;
    const tableBody = document.getElementById('compare-table-body');
    tableBody.innerHTML = '<tr><td colspan="5" style="text-align:center; padding:1.5rem; color:var(--accent-cyan);">Running benchmark tests...</td></tr>';
    document.getElementById('compare-modal').classList.remove('hidden');

    const results = [];

    for (const algo of algos) {
        const payload = {
            grid: {
                width: COLS,
                height: ROWS,
                walls: getWallsArray(),
                weights: getWeightsArray()
            },
            mode: mode,
            start: [startNode.row, startNode.col],
            end: [endNode.row, endNode.col],
            algorithm: algo
        };

        try {
            const response = await fetch('/api/solve', {
                method: 'POST',
                headers: { 'Content-Type': 'application/json' },
                body: JSON.stringify(payload)
            });
            const data = await response.json();
            results.push({ algo: algo, ...data });
        } catch (err) {
            console.error(err);
        }
    }

    tableBody.innerHTML = '';
    results.forEach(res => {
        const row = document.createElement('tr');
        const stats = res.stats || {};
        const visited = stats.nodesVisited !== undefined ? stats.nodesVisited : '-';
        const len = stats.pathLength !== undefined ? stats.pathLength : '-';
        const timeVal = stats.executionTimeMs !== undefined ? stats.executionTimeMs.toFixed(2) : (stats.timeMs ? stats.timeMs.toFixed(2) : '-');
        const costVal = stats.pathCost !== undefined ? stats.pathCost.toFixed(1) : len;

        row.innerHTML = `
            <td><strong>${res.algo}</strong></td>
            <td>${visited}</td>
            <td>${len}</td>
            <td>${costVal}</td>
            <td>${timeVal} ms</td>
        `;
        tableBody.appendChild(row);
    });
}

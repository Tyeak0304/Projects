(() => {
  const canvas = document.getElementById('gol-canvas');
  const ctx = canvas.getContext('2d');

  const CELL = 14;
  const ALIVE_COLOR = 'rgba(88, 166, 255, 0.35)';
  const DEAD_COLOR  = 'transparent';
  const TICK_MS     = 120;

  let cols, rows, grid, next;

  function resize() {
    canvas.width  = window.innerWidth;
    canvas.height = window.innerHeight;
    cols = Math.floor(canvas.width  / CELL);
    rows = Math.floor(canvas.height / CELL);
    grid = randomGrid();
    next = emptyGrid();
  }

  function emptyGrid() {
    return Array.from({ length: rows }, () => new Uint8Array(cols));
  }

  function randomGrid() {
    return Array.from({ length: rows }, () =>
      Uint8Array.from({ length: cols }, () => (Math.random() < 0.3 ? 1 : 0))
    );
  }

  function step() {
    for (let r = 0; r < rows; r++) {
      for (let c = 0; c < cols; c++) {
        let n = 0;
        for (let dr = -1; dr <= 1; dr++) {
          for (let dc = -1; dc <= 1; dc++) {
            if (dr === 0 && dc === 0) continue;
            const nr = (r + dr + rows) % rows;
            const nc = (c + dc + cols) % cols;
            n += grid[nr][nc];
          }
        }
        const alive = grid[r][c];
        next[r][c] = alive
          ? (n === 2 || n === 3 ? 1 : 0)
          : (n === 3 ? 1 : 0);
      }
    }
    [grid, next] = [next, grid];
  }

  function draw() {
    ctx.clearRect(0, 0, canvas.width, canvas.height);
    ctx.fillStyle = ALIVE_COLOR;
    for (let r = 0; r < rows; r++) {
      for (let c = 0; c < cols; c++) {
        if (grid[r][c]) {
          ctx.fillRect(c * CELL + 1, r * CELL + 1, CELL - 2, CELL - 2);
        }
      }
    }
  }

  window.addEventListener('resize', resize, { passive: true });
  resize();

  let last = 0;
  function loop(ts) {
    if (ts - last >= TICK_MS) {
      step();
      draw();
      last = ts;
    }
    requestAnimationFrame(loop);
  }
  requestAnimationFrame(loop);

  // Click to toggle cells
  canvas.addEventListener('click', e => {
    const c = Math.floor(e.clientX / CELL);
    const r = Math.floor(e.clientY / CELL);
    if (r >= 0 && r < rows && c >= 0 && c < cols) {
      grid[r][c] ^= 1;
    }
  });
})();

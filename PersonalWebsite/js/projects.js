(() => {
  const grid = document.getElementById('projects-grid');
  const filters = document.querySelectorAll('.filter-btn');
  let allProjects = [];
  let activeFilter = 'all';

  async function load() {
    try {
      const res = await fetch('data/projects.json');
      if (!res.ok) throw new Error('fetch failed');
      allProjects = await res.json();
    } catch {
      grid.innerHTML = '<p class="no-projects">Could not load projects.json.</p>';
      return;
    }
    render();
  }

  function render() {
    const filtered = activeFilter === 'all'
      ? allProjects
      : allProjects.filter(p => p.status === activeFilter);

    if (filtered.length === 0) {
      grid.innerHTML = '<p class="no-projects">No projects found.</p>';
      return;
    }

    grid.innerHTML = filtered.map(p => {
      const badgeClass = p.status === 'completed' ? 'badge-completed' : 'badge-in-progress';
      const badgeLabel = p.status === 'completed' ? 'Completed' : 'In Progress';
      const tags = (p.tags || []).map(t => `<span class="tag">${t}</span>`).join('');
      const link = p.link
        ? `<a class="project-link" href="${p.link}" target="_blank" rel="noopener">
             View project ↗
           </a>`
        : '';
      return `
        <article class="project-card">
          <div class="project-card-header">
            <h3>${p.title}</h3>
            <span class="badge ${badgeClass}">${badgeLabel}</span>
          </div>
          <p>${p.description}</p>
          <div class="project-tags">${tags}</div>
          ${link}
        </article>`;
    }).join('');
  }

  filters.forEach(btn => {
    btn.addEventListener('click', () => {
      filters.forEach(b => b.classList.remove('active'));
      btn.classList.add('active');
      activeFilter = btn.dataset.filter;
      render();
    });
  });

  load();
})();

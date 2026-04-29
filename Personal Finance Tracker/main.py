from fastapi import FastAPI, HTTPException
from fastapi.staticfiles import StaticFiles
from fastapi.responses import FileResponse
from pydantic import BaseModel
from typing import Optional
import sqlite3
import os
from datetime import date

BASE_DIR = os.path.dirname(os.path.abspath(__file__))
DB_PATH = os.path.join(BASE_DIR, "finance.db")
STATIC_DIR = os.path.join(BASE_DIR, "static")

EXPENSE_CATEGORIES = [
    "Food & Dining", "Housing & Rent", "Transportation", "Entertainment",
    "Healthcare", "Shopping", "Utilities", "Personal Care", "Education",
    "Travel", "Subscriptions", "Savings", "Other"
]

def get_db():
    conn = sqlite3.connect(DB_PATH)
    conn.row_factory = sqlite3.Row
    conn.execute("PRAGMA journal_mode=WAL")
    return conn

def init_db():
    conn = get_db()
    try:
        conn.executescript("""
            CREATE TABLE IF NOT EXISTS transactions (
                id          INTEGER PRIMARY KEY AUTOINCREMENT,
                type        TEXT    NOT NULL CHECK(type IN ('income', 'expense')),
                amount      REAL    NOT NULL CHECK(amount > 0),
                category    TEXT    NOT NULL,
                description TEXT    NOT NULL,
                date        TEXT    NOT NULL,
                created_at  TEXT    DEFAULT (datetime('now'))
            );

            CREATE TABLE IF NOT EXISTS budgets (
                id            INTEGER PRIMARY KEY AUTOINCREMENT,
                category      TEXT    NOT NULL UNIQUE,
                monthly_limit REAL    NOT NULL CHECK(monthly_limit > 0),
                updated_at    TEXT    DEFAULT (datetime('now'))
            );

            CREATE INDEX IF NOT EXISTS idx_tx_date ON transactions(date);
            CREATE INDEX IF NOT EXISTS idx_tx_type ON transactions(type);
        """)
        conn.commit()
    finally:
        conn.close()


app = FastAPI(title="Personal Finance Tracker")


class TransactionIn(BaseModel):
    type: str
    amount: float
    category: str
    description: str
    date: str


class BudgetIn(BaseModel):
    category: str
    monthly_limit: float


@app.on_event("startup")
def startup():
    init_db()


# ── Static files ──────────────────────────────────────────────────────────────
app.mount("/static", StaticFiles(directory=STATIC_DIR), name="static")


@app.get("/")
def root():
    return FileResponse(os.path.join(STATIC_DIR, "index.html"))


# ── Transactions ──────────────────────────────────────────────────────────────
@app.get("/api/transactions")
def list_transactions(
    month: Optional[str] = None,
    type: Optional[str] = None,
    category: Optional[str] = None,
    limit: int = 500,
):
    conn = get_db()
    try:
        q = "SELECT * FROM transactions WHERE 1=1"
        p = []
        if month:
            q += " AND strftime('%Y-%m', date) = ?"
            p.append(month)
        if type:
            q += " AND type = ?"
            p.append(type)
        if category:
            q += " AND category = ?"
            p.append(category)
        q += " ORDER BY date DESC, created_at DESC LIMIT ?"
        p.append(limit)
        return [dict(r) for r in conn.execute(q, p).fetchall()]
    finally:
        conn.close()


@app.post("/api/transactions", status_code=201)
def create_transaction(tx: TransactionIn):
    if tx.type not in ("income", "expense"):
        raise HTTPException(400, "type must be 'income' or 'expense'")
    if tx.amount <= 0:
        raise HTTPException(400, "amount must be positive")
    conn = get_db()
    try:
        cur = conn.execute(
            "INSERT INTO transactions (type, amount, category, description, date) VALUES (?,?,?,?,?)",
            (tx.type, tx.amount, tx.category.strip(), tx.description.strip(), tx.date),
        )
        conn.commit()
        return dict(conn.execute("SELECT * FROM transactions WHERE id=?", (cur.lastrowid,)).fetchone())
    finally:
        conn.close()


@app.delete("/api/transactions/{tid}")
def delete_transaction(tid: int):
    conn = get_db()
    try:
        r = conn.execute("DELETE FROM transactions WHERE id=?", (tid,))
        conn.commit()
        if r.rowcount == 0:
            raise HTTPException(404, "Not found")
        return {"ok": True}
    finally:
        conn.close()


# ── Budgets ───────────────────────────────────────────────────────────────────
@app.get("/api/budgets")
def list_budgets():
    conn = get_db()
    try:
        return [dict(r) for r in conn.execute("SELECT * FROM budgets ORDER BY category").fetchall()]
    finally:
        conn.close()


@app.post("/api/budgets", status_code=201)
def upsert_budget(b: BudgetIn):
    if b.monthly_limit <= 0:
        raise HTTPException(400, "monthly_limit must be positive")
    conn = get_db()
    try:
        conn.execute(
            """INSERT INTO budgets (category, monthly_limit) VALUES (?,?)
               ON CONFLICT(category) DO UPDATE
               SET monthly_limit=excluded.monthly_limit, updated_at=datetime('now')""",
            (b.category.strip(), b.monthly_limit),
        )
        conn.commit()
        return dict(conn.execute("SELECT * FROM budgets WHERE category=?", (b.category,)).fetchone())
    finally:
        conn.close()


@app.delete("/api/budgets/{bid}")
def delete_budget(bid: int):
    conn = get_db()
    try:
        r = conn.execute("DELETE FROM budgets WHERE id=?", (bid,))
        conn.commit()
        if r.rowcount == 0:
            raise HTTPException(404, "Not found")
        return {"ok": True}
    finally:
        conn.close()


# ── Dashboard ─────────────────────────────────────────────────────────────────
@app.get("/api/dashboard")
def dashboard():
    conn = get_db()
    try:
        today = date.today()
        month = today.strftime("%Y-%m")
        day = today.day

        totals = conn.execute("""
            SELECT
                COALESCE(SUM(CASE WHEN type='income'  THEN amount ELSE 0 END),0) AS income,
                COALESCE(SUM(CASE WHEN type='expense' THEN amount ELSE 0 END),0) AS expenses
            FROM transactions WHERE strftime('%Y-%m', date)=?
        """, (month,)).fetchone()

        income   = totals["income"]
        expenses = totals["expenses"]
        net      = income - expenses
        savings_rate = round(net / income * 100, 1) if income > 0 else 0.0

        cat_rows = conn.execute("""
            SELECT category, SUM(amount) AS total
            FROM transactions
            WHERE type='expense' AND strftime('%Y-%m', date)=?
            GROUP BY category ORDER BY total DESC
        """, (month,)).fetchall()

        budgets = conn.execute("SELECT * FROM budgets").fetchall()
        budget_status = []
        for b in budgets:
            spent = conn.execute("""
                SELECT COALESCE(SUM(amount),0) AS s FROM transactions
                WHERE type='expense' AND category=? AND strftime('%Y-%m', date)=?
            """, (b["category"], month)).fetchone()["s"]
            pct = round(spent / b["monthly_limit"] * 100, 1) if b["monthly_limit"] else 0
            budget_status.append({
                "id": b["id"],
                "category": b["category"],
                "limit": b["monthly_limit"],
                "spent": spent,
                "remaining": max(0.0, b["monthly_limit"] - spent),
                "percent": pct,
                "over": spent > b["monthly_limit"],
            })

        recent = conn.execute("""
            SELECT * FROM transactions ORDER BY date DESC, created_at DESC LIMIT 8
        """).fetchall()

        trend = conn.execute("""
            SELECT strftime('%Y-%m', date) AS month,
                   COALESCE(SUM(CASE WHEN type='income'  THEN amount ELSE 0 END),0) AS income,
                   COALESCE(SUM(CASE WHEN type='expense' THEN amount ELSE 0 END),0) AS expenses
            FROM transactions
            WHERE date >= date('now','-5 months','start of month')
            GROUP BY month ORDER BY month
        """).fetchall()

        return {
            "month": month,
            "income": income,
            "expenses": expenses,
            "net": net,
            "savings_rate": savings_rate,
            "category_spending": [dict(r) for r in cat_rows],
            "budget_status": budget_status,
            "recent": [dict(r) for r in recent],
            "trend": [dict(r) for r in trend],
            "velocity": {
                "daily_avg": round(expenses / day, 2) if day else 0,
                "projected": round(expenses / day * 30, 2) if day else 0,
                "day": day,
            },
        }
    finally:
        conn.close()


# ── Analytics ─────────────────────────────────────────────────────────────────
@app.get("/api/analytics")
def analytics(months: int = 6):
    conn = get_db()
    try:
        summary = conn.execute(f"""
            SELECT strftime('%Y-%m', date) AS month,
                   COALESCE(SUM(CASE WHEN type='income'  THEN amount ELSE 0 END),0) AS income,
                   COALESCE(SUM(CASE WHEN type='expense' THEN amount ELSE 0 END),0) AS expenses
            FROM transactions
            WHERE date >= date('now','-{months} months','start of month')
            GROUP BY month ORDER BY month
        """).fetchall()

        top_cats = conn.execute("""
            SELECT category, SUM(amount) AS total, COUNT(*) AS count
            FROM transactions WHERE type='expense'
            GROUP BY category ORDER BY total DESC LIMIT 10
        """).fetchall()

        cat_monthly = conn.execute(f"""
            SELECT strftime('%Y-%m', date) AS month, category, SUM(amount) AS total
            FROM transactions
            WHERE type='expense'
              AND date >= date('now','-{months} months','start of month')
            GROUP BY month, category ORDER BY month, total DESC
        """).fetchall()

        return {
            "summary": [dict(r) for r in summary],
            "top_categories": [dict(r) for r in top_cats],
            "category_monthly": [dict(r) for r in cat_monthly],
        }
    finally:
        conn.close()


# ── Meta ──────────────────────────────────────────────────────────────────────
@app.get("/api/categories")
def categories():
    return EXPENSE_CATEGORIES


if __name__ == "__main__":
    import uvicorn
    uvicorn.run("main:app", host="127.0.0.1", port=8000, reload=True)
